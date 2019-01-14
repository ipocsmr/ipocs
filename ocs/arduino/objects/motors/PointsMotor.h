
#ifndef POINTSMOTOR_H
#define POINTSMOTOR_H

#include "../../../IPOCS/Packets/PointsStatusPacket.h"

class PointsMotor {
  public:
    virtual void handleOrder(IPOCS::Packet* basePacket) = 0;
    virtual void loop() = 0;
    virtual int objectInit(unsigned char configData[]) = 0;
    virtual IPOCS::PointsStatusPacket::E_RQ_POINTS_STATE getState() = 0;
};

typedef PointsMotor* (*initPointsMotorFunction)();

#endif
