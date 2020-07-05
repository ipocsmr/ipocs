#include "ArduinoConnection.h"
#include "ServerConnection.h"
#include "Configuration.h"
#include "http.h"
#include "../IPC/Message.h"
#include "../IPOCS/Message.h"

#define QUIET_TIMEOUT 1000
void onPacketReceived(const uint8_t* buffer, size_t size);

esp::ArduinoConnection& esp::ArduinoConnection::instance() {
    static esp::ArduinoConnection conn;
    return conn;
}

void esp::ArduinoConnection::begin() {
    this->m_ulLastMsg = millis();
    if (this->packetSerial != nullptr) {
      delete this->packetSerial;
    } 
    Serial.begin(115200);
    this->packetSerial = new SLIPPacketSerial();
    this->packetSerial->setStream(&Serial);
    this->packetSerial->setPacketHandler(&onPacketReceived);
}

void esp::ArduinoConnection::loop() {
  unsigned long ulCurrentTime = millis();
  if (ulCurrentTime - this->m_ulLastMsg >= QUIET_TIMEOUT) {
    this->begin();
  }
  if (this->packetSerial != nullptr) {
    this->packetSerial->update();
  }
}

void onPacketReceived(const uint8_t* buffer, size_t size)
{
  if (size < 4) {
    return;
  }
  if (!IPC::Message::verifyChecksum(buffer)) {
    esp::Http::instance().log("Message CRC didn't match");
  }
  esp::ArduinoConnection::instance().m_ulLastMsg = millis();
  // Make a temporary buffer.
  IPC::Message* ipcMsg = IPC::Message::create(buffer);
  switch(ipcMsg->RT_TYPE) {
    case IPC::STARTED: {
      esp::Http::instance().setArduinoVersion(ipcMsg->pld);
      IPC::Message* ipcSiteData = IPC::Message::create();
      ipcSiteData->RT_TYPE = IPC::SITEDATA;
      ipcSiteData->setPayload();
      if (Configuration::verifyCrc()) {
        uint8_t siteData[200];
        uint8_t siteDataLength = Configuration::getSD(siteData, 200);
        ipcSiteData->setPayload(siteData, siteDataLength);
      } else {
      }
      esp::ArduinoConnection::instance().send(ipcSiteData);
      delete ipcSiteData;
      if (esp::ServerConnection::instance().connected()) {
        IPC::Message* ipcConnected = IPC::Message::create();
        ipcConnected->RT_TYPE = IPC::CESTAB;
        ipcConnected->setPayload();
        esp::ArduinoConnection::instance().send(ipcConnected);
        delete ipcConnected;
      }
      break; }
    case IPC::SITEDATA: {
      // Not handled in ESP
      break; }
    case IPC::IPOCS: {
      IPOCS::Message* ipocsMsg = IPOCS::Message::create(ipcMsg->pld);
      esp::ServerConnection::instance().send(ipocsMsg);
      delete ipocsMsg;
      break; }
    case IPC::RESTART: {
      break; }
    case IPC::IPONG: {
      esp::Http::instance().setArduinoVersion(ipcMsg->pld);
      break; }
    case IPC::IPING: {
      break; }
    case IPC::CESTAB: {
      break; }
  }
  delete ipcMsg;
}

void esp::ArduinoConnection::send(const uint8_t* const msg, const size_t length) {
  if (this->packetSerial != nullptr) {
    this->packetSerial->send(msg, length);
  }
}

void esp::ArduinoConnection::send(const char* const msg, const size_t length) {
  this->send((const char* const)msg, length);
}

void esp::ArduinoConnection::send(IPC::Message* msg) {
    uint8_t buffer[msg->RL_MESSAGE + 2];
    size_t msgSize = msg->serialize(buffer);
    if (this->packetSerial != nullptr) {
      this->packetSerial->send(buffer, msgSize);
    }
}
