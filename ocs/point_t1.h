/*****************
 * Point - Type 1 - definition file.
 * 
 * This file defines how to operate the Point Type 1.
 * 
 * @author Fredrik Elestedt
 * @date
 * @history see the git log of the origin repository.
 * 
 */
#ifndef POINT_T1_H
#define POINT_T1_H

#include "BasicObject.h"
#include <Servo.h>

namespace IPOCS {
  class Packet;
}

struct PointT1Data {
  byte servoPin;
};

class PointT1: public BasicObject {
  public:
    PointT1();
    virtual void handleOrder(IPOCS::Packet* basePacket);
    virtual void update();
  protected:
    virtual void objectInit(byte configData[], int configDataLen);
  private:
    Servo object;
    struct PointT1Data data;
    int setPos;
    int curPos;
    long lastRun;
};

#endif

