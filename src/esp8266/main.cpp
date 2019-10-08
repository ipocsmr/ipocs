#include "http.h"
#include "Configuration.h"
#include "ServerConnection.h"
#include "../IPC/Message.h"
#include "../IPOCS/Message.h"
#include "ArduinoConnection.h"
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <list>

#define MIN_LOOP_TIME 100
#define PING_TIME 1000
#define WIFI_CONNECT 30000

std::list<WiFiEventHandler> wifiHandlers;

unsigned long connectionInitiated = 0;
int attemptNo = 0;
unsigned long lastPing = 0;

int onStationModeConnected(const WiFiEventStationModeConnected& change) {
  return 0;
}

int onStationModeDisconnected(const WiFiEventStationModeDisconnected& change) {
  esp::ServerConnection::instance().disconnect();
  return 0;
}

int onStationModeGotIP(const WiFiEventStationModeGotIP& change) {
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
  char name[20];
  sprintf(name, "ipocs_%d", Configuration::getUnitID());
  char wifi_name[20];
  sprintf(wifi_name, "ipocs_%03d_%06X", Configuration::getUnitID(), ESP.getChipId());
  WiFi.hostname(wifi_name);
  MDNS.begin(name);
  esp::Http::instance().setup();
  esp::ArduinoConnection::instance().begin();
  wifiHandlers.push_back(WiFi.onStationModeDisconnected(onStationModeDisconnected));
  lastPing = millis();
  connectionInitiated = 0;
}

void setupWiFi(void) {
  char wifi_name[20];
  sprintf(wifi_name, "ipocs_%03d_%06X", Configuration::getUnitID(), ESP.getChipId());
  char ssid[33];
  Configuration::getSSID(ssid);
  char password[61];
  Configuration::getPassword(password);
  esp::Http::instance().log("Connecting to SSID: " + String(ssid));
  WiFi.disconnect();
  WiFi.hostname(wifi_name);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
}

void loop(void)
{
  MDNS.update();
  int wiFiStatus = WiFi.status();

  if (wiFiStatus != WL_CONNECTED)
  {
    if (connectionInitiated == 0)
    {
      connectionInitiated = millis();
      setupWiFi();
    }
    if ((millis() - connectionInitiated) >= WIFI_CONNECT)
    {
      // WiFi failed to reconnect - go into soft AP mode
      WiFi.mode(WIFI_AP);
      char name[20];
      sprintf(name, "IPOCS_%04X", Configuration::getUnitID());
      WiFi.softAP(name);
      connectionInitiated = 0;
      delay(500); // Let the AP settle
    }
  }
  else
  {
    if (connectionInitiated != 0)
    {
      // WiFi connected. Don't care if it's AP or Station mode.
      connectionInitiated = 0;
      esp::Http::instance().log(String("IP address: ") + WiFi.localIP().toString());
      MDNS.notifyAPChange();
    }
  }

  esp::Http::instance().loop();
  esp::ServerConnection::instance().loop(wifi_station_get_connect_status() == STATION_GOT_IP);
  esp::ArduinoConnection::instance().loop();

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
