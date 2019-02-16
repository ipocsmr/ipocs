#include <SPI.h>

#include "PointsMotorStore.h"
#include "PointsMotor_Pulse.h"
#include "../../log.h"

const int NO_POSITION_INPUT = 100;

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
  this->throwLeftOutput = configData[1];
  this->throwRightOutput = configData[2];
  this->posInput = configData[3];

  this->lastOrderState = (IPOCS::ThrowPointsPacket::E_RQ_POINTS_COMMAND)0;
  this->lastOrderMillis = 0;
  pinMode(this->throwLeftOutput, OUTPUT);
  pinMode(this->throwRightOutput, OUTPUT);
  digitalWrite(this->throwLeftOutput, LOW);
  digitalWrite(this->throwRightOutput, LOW);
  return 4;
}

void PointsMotor_Pulse::handleOrder(IPOCS::Packet* basePacket)
{
  Log log1("Order MSG");
  if (basePacket->RNID_PACKET == 10)
  {
    LOG("throw order");
    IPOCS::ThrowPointsPacket* packet = (IPOCS::ThrowPointsPacket*)basePacket;
    this->lastOrderMillis = millis();
    this->lastOrderState = packet->RQ_POINTS_COMMAND;
    unsigned int pinToSet = 0;
    switch (packet->RQ_POINTS_COMMAND)
    {
      case IPOCS::ThrowPointsPacket::DIVERT_LEFT: pinToSet = this->throwLeftOutput; break;
      case IPOCS::ThrowPointsPacket::DIVERT_RIGHT: pinToSet = this->throwRightOutput; break;
      default:
        // TODO: Send error about invalid value
        LOG("Unknown command");
        break;
    }
    digitalWrite(pinToSet, HIGH);
  }
}

void PointsMotor_Pulse::loop()
{
  if (this->lastOrderMillis == 0)
    return;
  IPOCS::PointsStatusPacket::E_RQ_POINTS_STATE currentState = this->getState();
  bool bSetLow = ((this->posInput >= NO_POSITION_INPUT) &&
    (currentState != IPOCS::PointsStatusPacket::E_RQ_POINTS_STATE::MOVING)) ||
    (currentState == IPOCS::PointsStatusPacket::E_RQ_POINTS_STATE::MOVING);
  if (bSetLow)
  {
    unsigned int timeToKeepActive =  500U;
    if (millis() - this->lastOrderMillis > timeToKeepActive)
    {
      digitalWrite(this->throwLeftOutput, LOW);
      digitalWrite(this->throwRightOutput, LOW);
      this->lastOrderMillis = 0;
    }
  }
}

IPOCS::PointsStatusPacket::E_RQ_POINTS_STATE PointsMotor_Pulse::getState()
{
  IPOCS::PointsStatusPacket::E_RQ_POINTS_STATE pos = IPOCS::PointsStatusPacket::E_RQ_POINTS_STATE::OUT_OF_CONTROL;
  if (this->posInput >= NO_POSITION_INPUT) {
    unsigned int timeToKeepActive =  500U;
    if (millis() - this->lastOrderMillis > timeToKeepActive)
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
    } else if (posValue <= StateRight) {
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
