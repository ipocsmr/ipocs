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
#ifndef POINTSMOTOR_SERVO_H
#define POINTSMOTOR_SERVO_H

#include "PointsMotor.h"
#include <Servo.h>
#include "IPOCS.h"

class PointsMotor_Servo: public PointsMotor {
  public:
    PointsMotor_Servo();
    virtual void handleOrder(IPOCS::Packet* basePacket);
    virtual void update();
    virtual int objectInit(byte configData[]);
    virtual IPOCS::PointsStatusPacket::E_RQ_POINTS_STATE getState();
  private:
    Servo object;
    int posInput;
    
    int setPos;
    int curPos;
    long lastRun;
};

#endif

