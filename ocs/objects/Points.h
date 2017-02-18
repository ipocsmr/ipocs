/*****************
 * Generic Point Type - definition file.
 *
 * This file defines how to operate the Point Type 1.
 *
 * @author Fredrik Elestedt
 * @date
 * @history see the git log of the origin repository.
 *
 */
#ifndef POINTS_H
#define POINTS_H

#include "../BasicObject.h"
#include "../IPOCS.h"

class PointsMotor;

class Points: public BasicObject {
  public:
    Points();
    virtual void handleOrder(IPOCS::Packet* basePacket);
    virtual void loop();
  protected:
    virtual void objectInit(byte configData[], int configDataLen);
  private:
    struct PointsMotorNode {
      PointsMotor* motor;
      PointsMotorNode* next;
    };
    PointsMotorNode* first;
    IPOCS::PointsStatusPacket::E_RQ_POINTS_STATE lastSentState;
    int frogOutput;
};

#endif
