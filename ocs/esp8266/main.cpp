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
  String name = "ipocs_" + String(ESP.getChipId());
  MDNS.begin(name.c_str());
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
      if (attemptNo < 5)
      {
        String ssidString = Configuration::getSSID();
        char ssid[ssidString.length() + 1];
        ssidString.toCharArray(ssid, ssidString.length() + 1);
        String pwdString = Configuration::getPassword();
        char password[pwdString.length() + 1];
        pwdString.toCharArray(password, pwdString.length() + 1);
        esp::Http::instance().log("Connecting to SSID: " + ssidString);
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, password);
        attemptNo++;
      }
      else if (attemptNo < 6)
      {
        esp::Http::instance().log(String("Starting SoftAP"));
        WiFi.mode(WIFI_AP);
        WiFi.softAP("IPOCS_Default");
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
  esp::ArduinoConnection::instance().log("+");
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
