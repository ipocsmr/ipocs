#include "http.h"
#include "Configuration.h"
#include "ServerConnection.h"
#include "../IPC/Message.h"
#include "../IPOCS/Message.h"
#include "ArduinoConnection.h"
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>

#define MIN_LOOP_TIME 100

int onStationModeConnected(const WiFiEventStationModeConnected& change) {
  return 0;
}

int onStationModeDisconnected(const WiFiEventStationModeDisconnected& change) {
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
  MDNS.begin(name);
  esp::Http::instance().setup();
  esp::ArduinoConnection::instance().begin();
  WiFi.onStationModeConnected(onStationModeConnected);
  WiFi.onStationModeDisconnected(onStationModeDisconnected);
  WiFi.onStationModeGotIP(onStationModeGotIP);
  WiFi.onSoftAPModeStationConnected(onSoftAPModeStationConnected);
  WiFi.onSoftAPModeStationDisconnected(onSoftAPModeStationDisconnected);
}


int connectionInitiated = 0;
int attemptNo = 0;

void loop(void)
{
  MDNS.update();
  int loopStart = millis();
  int wiFiStatus = WiFi.status();
  if (wiFiStatus != WL_CONNECTED)
  {
    if ((millis() - connectionInitiated) > 10000)
    {
      MDNS.notifyAPChange();
      connectionInitiated = millis();
      if (attemptNo < 2)
      {
        char ssid[33];
        Configuration::getSSID(ssid);
        char password[61];
        Configuration::getPassword(password);
        esp::Http::instance().log("Connecting to SSID: " + String(ssid));
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, password);
        attemptNo++;
      }
      else if (attemptNo < 3)
      {
        esp::Http::instance().log(String("Starting SoftAP"));
        WiFi.mode(WIFI_AP);
        char name[20];
        sprintf(name, "IPOCS_%04X", Configuration::getUnitID());
        WiFi.softAP(name);
        attemptNo = 6;
      }
    }
  }
  else
  {
    if (attemptNo == 6 && connectionInitiated != 0 && (millis() - connectionInitiated) <= 10000)
    {
      attemptNo = 0;
      connectionInitiated = 0;
      esp::Http::instance().log(String("IP address: ") + WiFi.localIP().toString());
      MDNS.notifyAPChange();
    }
  }
  esp::Http::instance().loop();
  esp::ServerConnection::instance().loop(wifi_station_get_connect_status() == STATION_GOT_IP);
  esp::ArduinoConnection::instance().loop();
  // Make sure we're not working too fast for ESP
  int loopEnd = millis();
  if ((loopEnd - loopStart) < MIN_LOOP_TIME) {
    delay(MIN_LOOP_TIME - (loopEnd - loopStart));
  }

  IPC::Message* ipcPing = IPC::Message::create();
  ipcPing->RT_TYPE = IPC::IPING;
  ipcPing->setPayload();
  esp::ArduinoConnection::instance().send(ipcPing);
  delete ipcPing;
}
