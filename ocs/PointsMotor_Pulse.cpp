#include <SPI.h>

#include "PointsMotorStore.h"
#include "PointsMotor_Pulse.h"
#include "ServerConnection.h"
#include "IPOCS.h"

const int NO_POSITION_INPUT = 99;

// Left state is less than or equal to this value.
const int StateLeft = 399;
// Moving/Out Of Control state is less than or equal to this value (Out Of Control after 10s)
const int StateMoving = 799;
// Right state is less than or equal to this value.
const int StateRight = 1023;

PointsMotor_Pulse::PointsMotor_Pulse()
{
}

int PointsMotor_Pulse::objectInit(byte configData[])
{
  this->throwLeftOutput = configData[1] - 1;
  this->throwRightOutput = configData[2] - 1;
  this->posInput = configData[3] - 1;

  this->lastOrderState = (IPOCS::ThrowPointsPacket::E_RQ_POINTS_COMMAND)0;
  this->lastOrder = 0;
  digitalWrite(this->throwLeftOutput, LOW);
  digitalWrite(this->throwRightOutput, LOW);
  return 4;
}

void PointsMotor_Pulse::handleOrder(IPOCS::Packet* basePacket)
{
  if (basePacket->RNID_PACKET == 10)
  {
    IPOCS::ThrowPointsPacket* packet = (IPOCS::ThrowPointsPacket*)basePacket;
    this->lastOrder = millis();
    this->lastOrderState = packet->RQ_POINTS_COMMAND;
    switch (packet->RQ_POINTS_COMMAND)
    {
      case IPOCS::ThrowPointsPacket::DIVERT_LEFT: digitalWrite(throwLeftOutput, HIGH); break;
      case IPOCS::ThrowPointsPacket::DIVERT_RIGHT: digitalWrite(throwRightOutput, HIGH); break;
      default:
        // TODO: Send error about invalid value
        break;
    }
  }
}

void PointsMotor_Pulse::update()
{
  if (this->lastOrder != 0)
  {
    if (this->posInput >= NO_POSITION_INPUT)
    {
      if (this->getState() != IPOCS::PointsStatusPacket::E_RQ_POINTS_STATE::MOVING)
      {
        digitalWrite(this->throwLeftOutput, LOW);
        digitalWrite(this->throwRightOutput, LOW);
        this->lastOrder = 0;
      }
    }
    else
    {
      if (this->getState() == IPOCS::PointsStatusPacket::E_RQ_POINTS_STATE::MOVING)
      {
        digitalWrite(this->throwLeftOutput, LOW);
        digitalWrite(this->throwRightOutput, LOW);
        this->lastOrder = 0;
      }
    }
  }
}

IPOCS::PointsStatusPacket::E_RQ_POINTS_STATE PointsMotor_Pulse::getState()
{
  IPOCS::PointsStatusPacket::E_RQ_POINTS_STATE pos = IPOCS::PointsStatusPacket::E_RQ_POINTS_STATE::OUT_OF_CONTROL;
  if (this->posInput >= NO_POSITION_INPUT) {
    unsigned int timeToKeepActive = (this->posInput - NO_POSITION_INPUT) * 100U;
    if (millis() - this->lastOrder > timeToKeepActive)
    {
      switch (this->lastOrderState)
      {
        case IPOCS::ThrowPointsPacket::DIVERT_LEFT: pos = IPOCS::PointsStatusPacket::E_RQ_POINTS_STATE::LEFT; break;
        case IPOCS::ThrowPointsPacket::DIVERT_RIGHT: pos = IPOCS::PointsStatusPacket::E_RQ_POINTS_STATE::RIGHT; break;
        default:
          // TODO: Send error about invalid value
          break;
      }
    }
    else pos = IPOCS::PointsStatusPacket::E_RQ_POINTS_STATE::MOVING;
    // TODO Add 10s timeout
  } else {
    int posValue = analogRead(this->posInput);
    if (posValue < StateLeft) {
      pos = IPOCS::PointsStatusPacket::E_RQ_POINTS_STATE::LEFT;
    } else if (posValue < StateMoving) {
      // TODO Add 10s timeout
      pos = IPOCS::PointsStatusPacket::E_RQ_POINTS_STATE::MOVING;
    } else if (posValue < StateRight) {
      pos = IPOCS::PointsStatusPacket::E_RQ_POINTS_STATE::RIGHT;
    }
  }
  return pos;
}

static PointsMotor* createPointsMotor_Pulse()
{
  return new PointsMotor_Pulse();
}

__attribute__((constructor))
static void initialize_pointsmotor_pulse() {
  PointsMotorStore::getInstance().registerType(1, &createPointsMotor_Pulse);
}
