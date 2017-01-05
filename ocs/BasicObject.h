/*****************
 * Object interface definition file.
 * 
 * This file defines how to operate the a object.
 * 
 * @author Fredrik Elestedt
 * @date
 * @history see the git log of the origin repository.
 */
#ifndef BASICOBJECT_H
#define BASICOBJECT_H

#include <SPI.h>

class BasicObject {
  public:
    void init(String objectName, byte configData[], int configDataLen);
    virtual byte getStatus() = 0;
    virtual void handleOrder(byte orderVec[], int orderVecLen) = 0;
    virtual void update() = 0;
    virtual bool hasName(String objectName);
  protected:
    virtual void objectInit(byte configData[], int configDataLen) = 0;
    String objectName;
};

#endif

