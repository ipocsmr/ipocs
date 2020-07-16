#include "http.h"
#include "Configuration.h"
#include "ServerConnection.h"
#include "../IPC/Message.h"
#include "../IPOCS/Message.h"
#include "ArduinoConnection.h"
#include "LedControl.hpp"
#include "ArduinoFlash.h"
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <list>
#include <EEPROM.h>

#define MIN_LOOP_TIME 100
#define PING_TIME 500
#define WIFI_CONNECT 30000

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
  char name[20];
  sprintf(name, "ipocs_%03d", Configuration::getUnitID());
  MDNS.begin(name, WiFi.localIP());
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
  esp::LedControl::instance().begin();
  char name[20];
  sprintf(name, "ipocs_%03d", Configuration::getUnitID());
  WiFi.setOutputPower(20.5);
  WiFi.hostname(name);
  MDNS.begin(name);
  MDNS.addService("http", "tcp", 80);
  esp::Http::instance().setup();
  esp::ArduinoConnection::instance().begin();
  wifiHandlers.push_back(WiFi.onStationModeDisconnected(onStationModeDisconnected));
  wifiHandlers.push_back(WiFi.onStationModeGotIP(onStationModeGotIP));
  lastPing = millis();
}

void setupWiFi(void) {
  char name[20];
  sprintf(name, "ipocs_%03d", Configuration::getUnitID());
  char password[61];
  Configuration::getPassword(password);
  char cSSID[33];
  Configuration::getSSID(cSSID);
  esp::Http::instance().log("Connecting to SSID: " + String(cSSID));
  WiFi.disconnect();
  WiFi.hostname(name);
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
    esp::LedControl::instance().setState(BLINK, 100);
    setupWiFi();
  }
  if (wiFiStatus != WL_CONNECTED)
  {
    if (wifiMode == Normal)
    {
      if (connectionInitiated != 0 && (millis() - connectionInitiated) >= WIFI_CONNECT)
      {
        esp::LedControl::instance().setState(BLINK, 500);
        // WiFi failed to reconnect - go into soft AP mode
        WiFi.mode(WIFI_AP);
        char name[20];
        sprintf(name, "IPOCS_%03d", Configuration::getUnitID());
        WiFi.softAP(name);
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
      esp::LedControl::instance().setState(BLINK, 1000);
      // WiFi connected. Don't care if it's AP or Station mode.
      connectionInitiated = 0;
      esp::Http::instance().log(String("IP address: ") + WiFi.localIP().toString());
      MDNS.notifyAPChange();
    }
  }

  esp::ArduinoFlash::instance().loop();
  esp::LedControl::instance().loop();
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
