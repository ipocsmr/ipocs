#include "../communications/Configuration.h"
#include "../communications/ArduinoConnection.h"
#include "../communications/ServerConnection.h"
#include "../controller/EspConnection.h"
#include "../controller/ObjectStore.h"
#include "../IPC/Message.h"
#include "http.h"
#include "../IPOCS/Message.h"
#include "../IPOCS/Packets/ConnectionRequestPacket.h"

ard::EspConnection& ard::EspConnection::instance() {
    static ard::EspConnection conn;
    return conn;
}

void ard::EspConnection::begin() {
}

void ard::EspConnection::loop() {
}

void ard::EspConnection::log(String& str) {
  /*IPC::Message* ipcMsg = IPC::Message::create();
  ipcMsg->RT_TYPE = IPC::LOG;
  ipcMsg->setPayload((uint8_t* const)str.c_str(), strlen(str.c_str()) + 1);
  ard::EspConnection::instance().send(ipcMsg, false);
  delete ipcMsg;
  */
}

void ard::EspConnection::send(IPC::Message* ipcMsg, bool print) {
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
    case IPC::LOG: {
      esp::Http::instance().log(String((char*)ipcMsg->pld));
      break; }
    case IPC::CESTAB: {
      break; }
  }
}