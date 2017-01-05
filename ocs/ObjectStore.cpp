
#include "ObjectStore.h"
#include "Configuration.h"

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

void ObjectStore::updateObjects()
{
  for (ObjectStoreNode* node = this->first; node != NULL; node = node->next)
  {
    node->object->update();
  }
}

void ObjectStore::handleOrder(String objectName, byte orderVec[], long orderVecLen)
{
  for (ObjectStoreNode* node = this->first; node != NULL; node = node->next)
  {
    if (node->object->hasName(objectName))
    {
      node->object->handleOrder(orderVec, orderVecLen);
    }
  }
}

void ObjectStore::loadSaved()
{
  byte sd[200];
  byte sdLength = Configuration::getSD(sd, 200);
  byte currPos = 0;
  int objNum = 0;
  while (sdLength > currPos)
  {
    byte sdObjectLength = sd[currPos];
    byte sdObjectType = sd[currPos + 1];
    if (sdObjectType < 10 && this.functions[sdObjectType] != NULL) {
      BasicObject* bo = this->functions[sdObjectType]();
      bo->init(String("Points " + String(objNum + 1)), sd + (currPos + 2), sdObjectLength - 1);
      ObjectStore::getInstance().addObject(bo);
    }
    currPos += sd[currPos] + 1;
    objNum++;
  }
}

void ObjectStore::registerType(int typeId, initObjectFunction fun)
{
  this->functions[typeId] = fun;
}

