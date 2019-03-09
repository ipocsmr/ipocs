#include "BasicObject.h"
#include <string.h>
#include "EspConnection.h"
#include "../IPC/Message.h"
#include "../IPOCS/Message.h"

void BasicObject::init(const char objectName[], const uint8_t* configData, int configDataLen)
{
  this->objectName = new char[strlen(objectName) + 1];
  strcpy(this->objectName, objectName);
  this->objectInit(configData, configDataLen);
}

bool BasicObject::hasName(const char* const objectName) {
  return strcmp(this->objectName, objectName) == 0;
}

void BasicObject::sendStatus() {
    IPOCS::Message* ipocsMsg = IPOCS::Message::create();
    ipocsMsg->setObject(this->objectName);
    IPOCS::Packet* pkt = this->getStatusPacket();
    ipocsMsg->setPacket(pkt);
    IPC::Message* ipcMsg = IPC::Message::create();
    ipcMsg->RT_TYPE = IPC::IPOCS;
    uint8_t message[100];
    uint8_t size = ipocsMsg->serialize(message);
    ipcMsg->setPayload(message, size);
    delete ipocsMsg;
    ard::EspConnection::instance().send(ipcMsg, true);
}