
#include "EspConnection.h"
#include "ObjectStore.h"
#include "../IPC/Message.h"
#include "../IPOCS/Message.h"
#include "../IPOCS/Packets/ConnectionRequestPacket.h"
#include "log.h"
#include <avr/wdt.h>
#include <uCRC16Lib.h>

const long reconnectTime = 1000;

typedef void(* ipocsResetFunc) (void);

void onPacketReceived(const uint8_t* buffer, size_t size);

ard::EspConnection& ard::EspConnection::instance() {
    static ard::EspConnection conn;
    return conn;
}

void ard::EspConnection::begin() {
    this->packetSerial = new SLIPPacketSerial();
#ifdef HAVE_HWSERIAL3
    Serial3.begin(115200);
    this->packetSerial->setStream(&Serial3);
#else
    Serial.begin(115200);
    this->packetSerial->begin(&Serial);
#endif
    this->packetSerial->setPacketHandler(&onPacketReceived);
    IPC::Message* ipcSiteData = IPC::Message::create();
    ipcSiteData->RT_TYPE = IPC::STARTED;
    uint8_t siteData[0];
    ipcSiteData->setPayload(siteData, 0);
    uint8_t buffer[ipcSiteData->RL_MESSAGE];
    size_t msgSize = ipcSiteData->serialize(buffer);
    this->packetSerial->send(buffer, msgSize);
    delete ipcSiteData;
}

void ard::EspConnection::loop() {
    this->packetSerial->update();
}

void onPacketReceived(const uint8_t* buffer, size_t size)
{
  if (size == 0) {
    return;
  }
  if (!IPC::Message::verifyChecksum(buffer)) {
    LOG("Message CRC didn't match");
    return;
  }
  // Make a temporary buffer.
  IPC::Message* ipcMsg = IPC::Message::create(buffer);
  switch(ipcMsg->RT_TYPE) {
    case IPC::STARTED: {
      // Not handled
      break; }
    case IPC::SITEDATA: {
      if (ipcMsg->RL_MESSAGE - 2 != 0) {
        LOG(String("Site Data Length is " + String(ipcMsg->RL_MESSAGE - 2) + ", loading"));
        ObjectStore::getInstance().setup(ipcMsg->pld, ipcMsg->RL_MESSAGE - 2);
      }
      else {
        LOG("Site Data Length is 0, not loading");
      }
      break; }
    case IPC::IPOCS: {
      IPOCS::Message* ipocsMsg = IPOCS::Message::create(ipcMsg->pld);
      ObjectStore::getInstance().handleOrder(ipocsMsg);
      delete ipocsMsg;
      break; }
    case IPC::RESTART: {
      LOG("Restarting...");
      wdt_enable(WDTO_15MS);  
      for(;;) { }     
      break; }
    case IPC::IPING: {
      LOGN(".");
      IPC::Message* ipcPong = IPC::Message::create();
      ipcPong->RT_TYPE = IPC::IPONG;
      ipcPong->setPayload();
      ard::EspConnection::instance().send(ipcPong, false);
      delete ipcPong;
      break; }
    case IPC::IPONG: {
      break; }
    case IPC::ILOG: {
      LOGN((const char*)ipcMsg->pld);
      break; }
  }
  delete ipcMsg;
}

void ard::EspConnection::send(IPC::Message* msg, bool print) {
    uint8_t buffer[msg->RL_MESSAGE + 10];
    size_t msgSize = msg->serialize(buffer);
    this->packetSerial->send(buffer, msgSize);
}