
#include "ObjectStore.h"

ObjectStore::ObjectStore()
{
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

