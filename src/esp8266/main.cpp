#include "http.h"
#include "../communications/Configuration.h"
#include "../communications/ServerConnection.h"
#include "../IPC/Message.h"
#include "../IPOCS/Message.h"
#include "../LedControl.h"
#include "../communications/ArduinoConnection.h"
#include "ArduinoFlash.h"
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <list>
#include <EEPROM.h>

#define MIN_LOOP_TIME 100
#define PING_TIME 500
#define WIFI_CONNECT 30000

extern const uint8_t LED_PIN = 14;

std::list<WiFiEventHandler> wifiHandlers;

enum EWiFiMode {
  None,
  Normal,
  AP
};

unsigned long connectionInitiated = 0;
enum EWiFiMode wifiMode = None;
int attemptNo = 0;
unsigned long lastPing = 0;

int onStationModeConnected(const WiFiEventStationModeConnected& change) {
  return 0;
}

int onStationModeDisconnected(const WiFiEventStationModeDisconnected& change) {
  esp::ServerConnection::instance().disconnect();
  if (connectionInitiated == 0) {
    wifiMode = None;
  }
  return 0;
}

int onStationModeGotIP(const WiFiEventStationModeGotIP& change) {
  char unitName[32];
  Configuration::getUnitName(unitName);
  MDNS.begin(unitName, WiFi.localIP());
  return 0;
}

int onSoftAPModeStationConnected(const WiFiEventSoftAPModeStationConnected& change) {
  return 0;
}

int onSoftAPModeStationDisconnected(const WiFiEventSoftAPModeStationDisconnected& change) {
  return 0;
}

int onSoftAPModeProbeRequestReceived(const WiFiEventSoftAPModeProbeRequestReceived& change) {
  return 0;
}

void setup(void)
{
  EEPROM.begin(1024);
  LedControl::instance().begin();
  char unitName[32];
  Configuration::getUnitName(unitName);
  WiFi.setOutputPower(20.5);
  WiFi.hostname(unitName);
  MDNS.begin(unitName);
  MDNS.addService("http", "tcp", 80);
  MDNS.addServiceTxt("http", "tcp", "ipocs", "true");
  esp::Http::instance().setup();
  esp::ArduinoConnection::instance().begin();
  wifiHandlers.push_back(WiFi.onStationModeDisconnected(onStationModeDisconnected));
  wifiHandlers.push_back(WiFi.onStationModeGotIP(onStationModeGotIP));
  lastPing = millis();
}

void setupWiFi(void) {
  char unitName[32];
  Configuration::getUnitName(unitName);
  char password[61];
  Configuration::getPassword(password);
  char cSSID[33];
  Configuration::getSSID(cSSID);
  esp::Http::instance().log("Connecting to SSID: " + String(cSSID));
  WiFi.disconnect();
  WiFi.hostname(unitName);
  WiFi.mode(WIFI_STA);
  WiFi.begin((const char*)String(cSSID).c_str(), (const char*)String(password).c_str());
}

void loop(void)
{
  MDNS.update();
  int wiFiStatus = WiFi.status();

  if (wifiMode == None) {
    connectionInitiated = millis();
    wifiMode = Normal;
    LedControl::instance().setState(BLINK, 100);
    setupWiFi();
  }
  if (wiFiStatus != WL_CONNECTED)
  {
    if (wifiMode == Normal)
    {
      if (connectionInitiated != 0 && (millis() - connectionInitiated) >= WIFI_CONNECT)
      {
        char unitName[32];
        Configuration::getUnitName(unitName);
        LedControl::instance().setState(BLINK, 500);
        // WiFi failed to reconnect - go into soft AP mode
        WiFi.mode(WIFI_AP);
        WiFi.softAP(unitName);
        connectionInitiated = 0;
        wifiMode = AP;
        delay(500); // Let the AP settle
      }
    }
  }
  else
  {
    if (connectionInitiated != 0)
    {
      LedControl::instance().setState(BLINK, 1000);
      // WiFi connected. Don't care if it's AP or Station mode.
      connectionInitiated = 0;
      esp::Http::instance().log(String("IP address: ") + WiFi.localIP().toString());
      MDNS.notifyAPChange();
    }
  }

  esp::ArduinoFlash::instance().loop();
  LedControl::instance().loop();
  esp::Http::instance().loop();
  esp::ServerConnection::instance().loop(wifi_station_get_connect_status() == STATION_GOT_IP);
  esp::ArduinoConnection::instance().loop();
  MDNS.update();
  unsigned long loopEnd = millis();
  if ((loopEnd - lastPing) >= PING_TIME) {
    lastPing = loopEnd;
    IPC::Message* ipcPing = IPC::Message::create();
    ipcPing->RT_TYPE = IPC::IPING;
    ipcPing->setPayload();
    esp::ArduinoConnection::instance().send(ipcPing);
    delete ipcPing;
  }
}
