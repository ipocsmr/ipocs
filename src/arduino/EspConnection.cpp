
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "EspConnection.h"
#include "ObjectStore.h"
#include "../IPC/Message.h"
#include "../IPOCS/Message.h"
#include "../IPOCS/Packets/ConnectionRequestPacket.h"
#include "log.h"
#include <avr/wdt.h>
#include <uCRC16Lib.h>

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define VERSION_STRING TOSTRING(VERSION_RAW)
#define QUIET_TIMEOUT 1000

typedef void(* ipocsResetFunc) (void);

void onPacketReceived(const uint8_t* buffer, size_t size);

ard::EspConnection& ard::EspConnection::instance() {
    static ard::EspConnection conn;
    return conn;
}

void ard::EspConnection::begin() {
    this->m_ulLastMsg = millis();
    if (this->packetSerial != nullptr) {
      delete this->packetSerial;
    }
    this->packetSerial = new SLIPPacketSerial();
    Serial.begin(115200);
    this->packetSerial->setStream(&Serial);
    this->packetSerial->setPacketHandler(&onPacketReceived);
    IPC::Message* ipcSiteData = IPC::Message::create();
    ipcSiteData->RT_TYPE = IPC::STARTED;
    uint8_t siteData[strlen(VERSION_STRING) + 1];
    memcpy(siteData, VERSION_STRING, strlen(VERSION_STRING) + 1);
#ifdef HAVE_HWSERIAL1
    Serial1.println("Version: " + String(VERSION_STRING));
#endif
    ipcSiteData->setPayload(siteData, strlen(VERSION_STRING) + 1);
    uint8_t buffer[ipcSiteData->RL_MESSAGE];
    size_t msgSize = ipcSiteData->serialize(buffer);
    this->packetSerial->send(buffer, msgSize);
    delete ipcSiteData;
}

void ard::EspConnection::loop() {
  unsigned long ulCurrentTime = millis();
  if (ulCurrentTime - this->m_ulLastMsg >= QUIET_TIMEOUT) {
    this->begin();
  }
  this->packetSerial->update();
}

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

void onPacketReceived(const uint8_t* buffer, size_t size)
{
  if (size == 0) {
    return;
  }
  if (size < 4 || (!IPC::Message::verifyChecksum(buffer))) {
#ifdef HAVE_HWSERIAL1
    Serial1.write(buffer, size);
    Serial1.flush();
#endif
    return;
  }
  ard::EspConnection::instance().m_ulLastMsg = millis();
  // Make a temporary buffer.
  IPC::Message* ipcMsg = IPC::Message::create(buffer);
  switch(ipcMsg->RT_TYPE) {
    case IPC::STARTED: {
      // Not handled
      break; }
    case IPC::SITEDATA: {
      if (ipcMsg->RL_MESSAGE - 2 != 0) {
        ObjectStore::getInstance().setup(ipcMsg->pld, ipcMsg->RL_MESSAGE - 2);
      }
      else {
      }
      break; }
    case IPC::IPOCS: {
      IPOCS::Message* ipocsMsg = IPOCS::Message::create(ipcMsg->pld);
      ObjectStore::getInstance().handleOrder(ipocsMsg);
      delete ipocsMsg;
      break; }
    case IPC::RESTART: {
        typedef void (*do_reboot_t)(void);
        const do_reboot_t do_reboot = (do_reboot_t)((FLASHEND - 511) >> 1);

        cli(); TCCR0A = TCCR1A = TCCR2A = 0; // make sure interrupts are off and timers are reset.
        do_reboot();
      break; }
    case IPC::IPING: {
#ifdef HAVE_HWSERIAL1
      Serial1.print('p');
      Serial1.flush();
#endif
      IPC::Message* ipcPong = IPC::Message::create();
      ipcPong->RT_TYPE = IPC::IPONG;
      ipcPong->setPayload();
      ard::EspConnection::instance().send(ipcPong, false);
      delete ipcPong;
      break; }
    case IPC::IPONG: {
      break; }
    case IPC::CESTAB: {
#ifdef HAVE_HWSERIAL1
      Serial1.print('u');
      Serial1.flush();
#endif
      ObjectStore::getInstance().sendAllStatus();
      break; }
  }
  delete ipcMsg;
}

void ard::EspConnection::send(IPC::Message* msg, bool print) {
    uint8_t buffer[msg->RL_MESSAGE + 10];
    size_t msgSize = msg->serialize(buffer);
    this->packetSerial->send(buffer, msgSize);
}