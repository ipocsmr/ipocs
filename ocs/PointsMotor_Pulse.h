/*****************
 * PointMotor Pule - definition file.
 * 
 * @author Fredrik Elestedt
 * @date
 * @history see the git log of the origin repository.
 * 
 */
#ifndef POINTSMOTOR_PULSE_H
#define POINTSMOTOR_PULSE_H

#include "PointsMotor.h"
#include "IPOCS.h"

class PointsMotor_Pulse: public PointsMotor {
  public:
    PointsMotor_Pulse();
    virtual void handleOrder(IPOCS::Packet* basePacket);
    virtual void loop();
    virtual int objectInit(byte configData[]);
    virtual IPOCS::PointsStatusPacket::E_RQ_POINTS_STATE getState();
  private:
    int throwLeftOutput;
    int throwRightOutput;
    int posInput;
    
    long lastOrder;
    IPOCS::ThrowPointsPacket::E_RQ_POINTS_COMMAND lastOrderState;
};

#endif

