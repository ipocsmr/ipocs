#include "../communications/ArduinoConnection.h"
#include "../communications/Configuration.h"
#include "../controller/EspConnection.h"
#include "../controller/ObjectStore.h"
#include "http.h"
#include "../IPC/Message.h"
#include "../IPOCS/Message.h"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define VERSION_STRING TOSTRING(VERSION_RAW)
//typedef void(* ipocsResetFunc) (void);

esp::ArduinoConnection& esp::ArduinoConnection::instance() {
    static esp::ArduinoConnection conn;
    return conn;
}

void esp::ArduinoConnection::begin() {
}

void esp::ArduinoConnection::stop() {
}

void esp::ArduinoConnection::loop() {
}

void esp::ArduinoConnection::send(const uint8_t* const msg, const size_t length) {
}

void esp::ArduinoConnection::send(const char* const msg, const size_t length) {
}

void esp::ArduinoConnection::send(IPC::Message* ipcMsg) {
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
        //typedef void (*do_reboot_t)(void);
        //const do_reboot_t do_reboot = (do_reboot_t)((FLASHEND - 511) >> 1);

        //cli(); TCCR0A = TCCR1A = TCCR2A = 0; // make sure interrupts are off and timers are reset.
        //do_reboot();
      break; }
    case IPC::IPING: {
#ifdef HAVE_HWSERIAL1
      Serial1.print('p');
      Serial1.flush();
#endif
      uint8_t siteData[strlen(VERSION_STRING) + 1];
      memcpy(siteData, VERSION_STRING, strlen(VERSION_STRING) + 1);
      if (!ard::EspConnection::instance().bHasSentStarted) {
        ard::EspConnection::instance().bHasSentStarted = true;
        IPC::Message* ipcSiteData = IPC::Message::create();
        ipcSiteData->RT_TYPE = IPC::STARTED;
#ifdef HAVE_HWSERIAL1
        Serial1.println("Version: " + String(VERSION_STRING));
#endif
        ipcSiteData->setPayload(siteData, strlen(VERSION_STRING) + 1);
        ard::EspConnection::instance().send(ipcSiteData, false);
        delete ipcSiteData;
      } else {
        IPC::Message* ipcPong = IPC::Message::create();
        ipcPong->RT_TYPE = IPC::IPONG;
        ipcPong->setPayload(siteData, strlen(VERSION_STRING) + 1);
        ard::EspConnection::instance().send(ipcPong, false);
        delete ipcPong;
      }
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
}
