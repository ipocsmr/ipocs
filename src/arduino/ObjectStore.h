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

namespace IPOCS {
  class Message;
};

class ObjectStore {
  public:
    static ObjectStore& getInstance()
    {
      static ObjectStore instance;
      return instance;
    }

    void registerType(int typeId, initObjectFunction fun);
    void handleOrder(IPOCS::Message* msg);
    void setup(const uint8_t* sd, uint8_t sdLength);
    void sendAllStatus();
    void loop();
  private:
    struct ObjectStoreNode {
      ObjectStoreNode* prev;
      ObjectStoreNode* next;
      BasicObject* object;
    };

    initObjectFunction functions[30];
    struct ObjectStoreNode* first;
    struct ObjectStoreNode* last;
    long basicObjectCount;

    ObjectStore();
    ObjectStore(ObjectStore const&);              // Don't Implement
    void operator=(ObjectStore const&); // Don't implement
    void addObject(BasicObject* bo);
};

#endif
