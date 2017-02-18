
#include "ObjectStore.h"
#include "Configuration.h"
#include "ServerConnection.h"

ObjectStore::ObjectStore()
{
  memset(this->functions, '\0', 10);
  this->first = NULL;
  this->last = NULL;
  this->basicObjectCount = 0;
}

void ObjectStore::addObject(BasicObject* bo)
{
  ObjectStoreNode* node = new ObjectStoreNode();
  node->prev = this->last;
  node->next = NULL;
  node->object = bo;
  if (this->first == NULL) {
    this->first = node;
  } else {
    this->last->next = node;
  }
  this->last = node;
  this->basicObjectCount++;
}

void ObjectStore::loop()
{
  for (ObjectStoreNode* node = this->first; node != NULL; node = node->next)
  {
    node->object->loop();
  }
}

void ObjectStore::handleOrder(IPOCS::Message* msg)
{
  if (msg->RXID_OBJECT == String((char)Configuration::getUnitID()))
  {
      if (msg->packet->RNID_PACKET == 5) {
        IPOCS::ApplicationDataPacket* dataPkt = (IPOCS::ApplicationDataPacket* const)msg->packet;
        uint8_t dataLength = dataPkt->RL_PACKET - 5;
        Configuration::setSD(dataPkt->data, dataLength);
        ServerConnection::getInstance().stop();
        delay(1000);
        ipocsResetFunc f = 0;
        f();
      }
  }
  else
  {
    for (ObjectStoreNode* node = this->first; node != NULL; node = node->next)
    {
      if (node->object->hasName(msg->RXID_OBJECT))
      {
        node->object->handleOrder(msg->packet);
      }
    }
  }
}

void ObjectStore::setup()
{
  byte sd[200];
  byte sdLength = Configuration::getSD(sd, 200);
  byte currPos = 0;
  if (!Configuration::verifyCrc()) {
    return;
  }
  while (sdLength > currPos)
  {
    byte sdObjectType = sd[currPos];
    byte sdObjectLength = sd[currPos + 1];
    String objectName;
    for (uint8_t* firstChar = sd + 2; *firstChar != 0x00; firstChar++)
    {
      objectName += String((char)(*firstChar));
    }
    // 1 byte for object type + object length + object name + null byte
    uint8_t msgParsed = 1 + 1 + objectName.length() + 1;

    if (sdObjectType < 10 && this->functions[sdObjectType] != NULL) {

      BasicObject* bo = this->functions[sdObjectType]();

      bo->init(objectName, sd + msgParsed, sdObjectLength - msgParsed);
      ObjectStore::getInstance().addObject(bo);
    }
    currPos += sdObjectLength + 1;
  }
}

void ObjectStore::registerType(int typeId, initObjectFunction fun)
{
  this->functions[typeId] = fun;
}
