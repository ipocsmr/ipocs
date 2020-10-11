#include "http.h"
#include "../communications/Configuration.h"
#include "../communications/ServerConnection.h"
#include "../IPC/Message.h"
#include "../IPOCS/Message.h"
#include "../communications/ArduinoConnection.h"
#include "../LedControl.h"
#include "../controller/ObjectStore.h"
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiClient.h>
#include <list>
#include <EEPROM.h>
#include <nvs_flash.h>
#include <esp_task_wdt.h>

#define MIN_LOOP_TIME 100
#define PING_TIME 500
#define WIFI_CONNECT 30000

extern const uint8_t LED_PIN = 2;

enum EWiFiMode {
  None,
  Normal,
  AP
};

unsigned long connectionInitiated = 0;
enum EWiFiMode wifiMode = None;
int attemptNo = 0;
unsigned long lastPing = 0;

int onStationModeDisconnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  esp::ServerConnection::instance().disconnect();
  if (connectionInitiated == 0) {
    wifiMode = None;
  }
  return 0;
}

int onStationModeGotIP(WiFiEvent_t event, WiFiEventInfo_t info) {
  char unitName[32];
  Configuration::getUnitName(unitName);
  MDNS.begin(unitName);
  return 0;
}

TaskHandle_t serverConnectionTask;

void cpuLoop(void* parameter) {
  disableCore0WDT();
  for (;;) {
    //esp_task_wdt_reset();
    esp::ServerConnection::instance().loop((WiFi.getStatusBits() & STA_HAS_IP_BIT) == STA_HAS_IP_BIT);
  }
}

void setup(void)
{
  Serial.begin(115200);
  //Serial.printf("TESTING PRINT %s:%i\n", __FILE__, __LINE__);
  tcpip_adapter_init();
  esp_err_t err = nvs_flash_init();
  switch (err) {
    case ESP_OK: break;
  }
  EEPROM.begin(1024);
  LedControl::instance().begin();
  char unitName[32];
  Configuration::getUnitName(unitName);
  WiFi.setTxPower(WIFI_POWER_19_5dBm);
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.setHostname(unitName);
  MDNS.begin(unitName);
  MDNS.addService("http", "tcp", 80);
  MDNS.addServiceTxt("http", "tcp", "ipocs", "true");
  esp::Http::instance().setup();
  esp::ArduinoConnection::instance().begin();
  WiFi.onEvent(onStationModeDisconnected, SYSTEM_EVENT_STA_DISCONNECTED);
  WiFi.onEvent(onStationModeGotIP, SYSTEM_EVENT_STA_GOT_IP);
  lastPing = millis();

  xTaskCreatePinnedToCore(
      cpuLoop, /* Function to implement the task */
      "serverConnectionTask", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      0,  /* Priority of the task */
      &serverConnectionTask,  /* Task handle. */
      0); /* Core where the task should run */
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
  WiFi.setTxPower(WIFI_POWER_19_5dBm);
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.setHostname(unitName);
  WiFi.mode(WIFI_STA);
  if (strnlen(cSSID, 33) > 0) {
    WiFi.begin((const char*)String(cSSID).c_str(), (const char*)String(password).c_str());
  }
}

void loop(void)
{
  //MDNS.update();
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
      //MDNS.notifyAPChange();
    }
  }
  LedControl::instance().loop();
  esp::Http::instance().loop();
  esp::ArduinoConnection::instance().loop();
  ObjectStore::getInstance().loop();
  //MDNS.update();
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
