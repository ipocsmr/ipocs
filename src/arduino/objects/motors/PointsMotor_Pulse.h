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
#include "../../../IPOCS/Packets/PointsStatusPacket.h"
#include "../../../IPOCS/Packets/ThrowPointsPacket.h"

class PointsMotor_Pulse: public PointsMotor {
  public:
    PointsMotor_Pulse();
    virtual void handleOrder(IPOCS::Packet* basePacket);
    virtual void loop();
    virtual int objectInit(const uint8_t configData[]);
    virtual IPOCS::PointsStatusPacket::E_RQ_POINTS_STATE getState();
  private:
    bool invertStatus;
    bool lowToThrow;
    uint16_t timeToKeepActive;
    uint8_t throwLeftOutput;
    uint8_t throwRightOutput;
    uint8_t posInput;

    uint32_t lastOrderMillis;
    IPOCS::ThrowPointsPacket::E_RQ_POINTS_COMMAND lastOrderState;
};

#endif
