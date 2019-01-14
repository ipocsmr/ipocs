#include "ArduinoConnection.h"
#include "ServerConnection.h"
#include "Configuration.h"
#include "http.h"
#include "../IPC/Message.h"
#include "../IPOCS/Message.h"

/*
void array_to_string(const uint8_t array[], unsigned int len, char buffer[])
{
    for (unsigned int i = 0; i < len; i++)
    {
        byte nib1 = (array[i] >> 4) & 0x0F;
        byte nib2 = (array[i] >> 0) & 0x0F;
        buffer[i*3+0] = ' ';
        buffer[i*3+1] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
        buffer[i*3+2] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
    }
    buffer[len*3] = '\0';
}
*/

void onPacketReceived(const uint8_t* buffer, size_t size);

esp::ArduinoConnection& esp::ArduinoConnection::instance() {
    static esp::ArduinoConnection conn;
    return conn;
}

void esp::ArduinoConnection::begin() {
    pinMode(LED_BUILTIN, OUTPUT);
    this->packetSerial = new SLIPPacketSerial();
    this->packetSerial->begin(115200);
    this->packetSerial->setPacketHandler(&onPacketReceived);
}

void esp::ArduinoConnection::loop() {
    this->packetSerial->update();
}

void onPacketReceived(const uint8_t* buffer, size_t size)
{
  //char str[100] = "";
  //array_to_string((uint8_t*)buffer, size, str);
  //esp::Http::instance().log("Received message " + String(size) + " : " + String(str));
  if (size < 4) {
    return;
  }
  if (!IPC::Message::verifyChecksum(buffer)) {
    esp::Http::instance().log("Message CRC didn't match");
  }
  //digitalWrite(LED_BUILTIN, LOW);
  // Make a temporary buffer.
  IPC::Message* ipcMsg = IPC::Message::create(buffer);
  switch(ipcMsg->RT_TYPE) {
    case IPC::STARTED: {
      IPC::Message* ipcSiteData = IPC::Message::create();
      ipcSiteData->RT_TYPE = IPC::SITEDATA;
      ipcSiteData->setPayload();
      if (Configuration::verifyCrc()) {
        uint8_t siteData[200];
        uint8_t siteDataLength = Configuration::getSD(siteData, 200);
        //char str[100] = "";
        //array_to_string(siteData, siteDataLength, str);
        //esp::Http::instance().log("Checksum verified " + String(str));
        ipcSiteData->setPayload(siteData, siteDataLength);
      } else {
        digitalWrite(LED_BUILTIN, LOW);
        //esp::Http::instance().log("Checksum failed");
      }
      esp::ArduinoConnection::instance().send(ipcSiteData);
      delete ipcSiteData;
      break; }
    case IPC::SITEDATA:
      // Not handled in ESP
      break;
    case IPC::IPOCS: {
      IPOCS::Message* ipocsMsg = IPOCS::Message::create(ipcMsg->pld);
      esp::ServerConnection::instance().send(ipocsMsg);
      delete ipocsMsg;
      break; }
    case IPC::RESTART:
      break;
    case IPC::IPONG: {
      //esp::Http::instance().log("PONG");
      break; }
    case IPC::IPING:
      break;
  }
  delete ipcMsg;
  //digitalWrite(LED_BUILTIN, HIGH);
}

void esp::ArduinoConnection::send(IPC::Message* msg) {
    uint8_t buffer[msg->RL_MESSAGE + 2];
    size_t msgSize = msg->serialize(buffer);

    //char str[100] = "";
    //array_to_string(buffer, msgSize, str);
    //esp::Http::instance().log(String(str));

    this->packetSerial->send(buffer, msgSize);
}
