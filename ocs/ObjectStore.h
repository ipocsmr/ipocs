/*****************
 *  Object store
 *
 * @author Fredrik Elestedt
 * @date
 * @history see the git log of the origin repository.
 *
 */
#ifndef OBJECT_STORE_H
#define OBJECT_STORE_H

#include "BasicObject.h"

class ObjectStore {
  public:
    static ObjectStore& getInstance()
    {
      static ObjectStore instance;
      return instance;
    }

    void registerType(int typeId, initObjectFunction fun);

    void loadSaved();
    void addObject(BasicObject* bo);
    void handleOrder(IPOCS::Message* msg);
    void updateObjects();
  private:
    struct ObjectStoreNode {
      ObjectStoreNode* prev;
      ObjectStoreNode* next;
      BasicObject* object;
    };

    initObjectFunction functions[10];
    ObjectStore();
    ObjectStore(ObjectStore const&);              // Don't Implement
    void operator=(ObjectStore const&); // Don't implement

    struct ObjectStoreNode* first;
    struct ObjectStoreNode* last;
    long basicObjectCount;
};

#endif

