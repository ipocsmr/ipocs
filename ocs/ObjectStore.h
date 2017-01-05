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

    void addObject(BasicObject* bo);
    void handleOrder(String objectName, byte orderVec[], long orderVecLen);
    void updateObjects();
  private:
    struct ObjectStoreNode {
      ObjectStoreNode* prev;
      ObjectStoreNode* next;
      BasicObject* object;
    };
  
    ObjectStore();
    ObjectStore(ObjectStore const&);              // Don't Implement
    void operator=(ObjectStore const&); // Don't implement

    struct ObjectStoreNode* first;
    struct ObjectStoreNode* last;
    long basicObjectCount;
};

#endif

