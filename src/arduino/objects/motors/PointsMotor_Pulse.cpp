#include <SPI.h>

#include "PointsMotorStore.h"
#include "PointsMotor_Pulse.h"
#include "../../log.h"

const uint8_t NO_POSITION_INPUT = 0;
const unsigned long MOVEMENT_TIMEOUT = 100000;

// Left state is less than or equal to this value.
const uint16_t StateLeft = 399;
// Right state is larger than or equal to this value.
const uint16_t StateRight = 799;

PointsMotor_Pulse::PointsMotor_Pulse()
{
}

int PointsMotor_Pulse::objectInit(const uint8_t configData[])
{
  this->throwLeftOutput = configData[0] + 1;
  this->throwRightOutput = configData[1] + 1;
  switch (configData[2]) {
    case 1: this->posInput = A0; break;
    case 2: this->posInput = A1; break;
    case 3: this->posInput = A2; break;
    case 4: this->posInput = A3; break;
    case 5: this->posInput = A4; break;
    case 6: this->posInput = A5; break;
    case 7: this->posInput = A6; break;
    case 8: this->posInput = A7; break;
#ifdef PIN_A8
    case 9: this->posInput = A8; break;
#endif
#ifdef PIN_A9
    case 10: this->posInput = A9; break;
#endif
#ifdef PIN_A10
    case 11: this->posInput = A10; break;
#endif
#ifdef PIN_A11
    case 12: this->posInput = A11; break;
#endif
#ifdef PIN_A12
    case 13: this->posInput = A12; break;
#endif
#ifdef PIN_A13
    case 14: this->posInput = A13; break;
#endif
#ifdef PIN_A14
    case 15: this->posInput = A14; break;
#endif
#ifdef PIN_A15
    case 16: this->posInput = A15; break;
#endif
    default: this->posInput = NO_POSITION_INPUT; break;
  }
  this->invertStatus = (configData[3] == 1);
  this->lowToThrow = (configData[4] == 1);
  this->timeToKeepActive = configData[5] * 10;
  this->lastOrderState = (IPOCS::ThrowPointsPacket::E_RQ_POINTS_COMMAND)0;
  this->lastOrderMillis = 0;
  pinMode(this->throwLeftOutput, OUTPUT);
  pinMode(this->throwRightOutput, OUTPUT);
  digitalWrite(this->throwLeftOutput, this->lowToThrow ? HIGH : LOW);
  digitalWrite(this->throwRightOutput, this->lowToThrow ? HIGH : LOW);
  return 4;
}

void PointsMotor_Pulse::handleOrder(IPOCS::Packet* basePacket)
{
  if (basePacket->RNID_PACKET == 10)
  {
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
        break;
    }
    digitalWrite(pinToSet, this->lowToThrow ? LOW : HIGH);
  }
}

void PointsMotor_Pulse::loop()
{
  if (this->lastOrderMillis == 0)
    return;
  if (millis() - this->lastOrderMillis > this->timeToKeepActive)
  {
    digitalWrite(this->throwLeftOutput, this->lowToThrow ? HIGH : LOW);
    digitalWrite(this->throwRightOutput, this->lowToThrow ? HIGH : LOW);
    this->lastOrderMillis = 0;
  }
}

IPOCS::PointsStatusPacket::E_RQ_POINTS_STATE PointsMotor_Pulse::getState()
{
  IPOCS::PointsStatusPacket::E_RQ_POINTS_STATE pos = IPOCS::PointsStatusPacket::E_RQ_POINTS_STATE::OUT_OF_CONTROL;
  if (this->posInput == NO_POSITION_INPUT) {
    if (millis() - this->lastOrderMillis > this->timeToKeepActive)
    {
      switch (this->lastOrderState)
      {
        case IPOCS::ThrowPointsPacket::DIVERT_LEFT: pos = IPOCS::PointsStatusPacket::E_RQ_POINTS_STATE::LEFT; break;
        case IPOCS::ThrowPointsPacket::DIVERT_RIGHT: pos = IPOCS::PointsStatusPacket::E_RQ_POINTS_STATE::RIGHT; break;
        default:
          // TODO: Send error about invalid value
          break;
      }
    } else {
      pos = IPOCS::PointsStatusPacket::E_RQ_POINTS_STATE::MOVING;
    }
  } else {
    uint16_t posValue = analogRead(this->posInput);
    if (posValue <= StateLeft) {
      pos = IPOCS::PointsStatusPacket::E_RQ_POINTS_STATE::LEFT;
    } else if (posValue >= StateRight) {
      pos = IPOCS::PointsStatusPacket::E_RQ_POINTS_STATE::RIGHT;
    } else {
      if (millis() - this->lastOrderMillis >= 10000) {
        pos = IPOCS::PointsStatusPacket::E_RQ_POINTS_STATE::MOVING;
      } else {
        pos = IPOCS::PointsStatusPacket::E_RQ_POINTS_STATE::OUT_OF_CONTROL;
      }
    }
    if (this->invertStatus && pos <= 2) {
      pos = (IPOCS::PointsStatusPacket::E_RQ_POINTS_STATE)((pos % 2) + 1);
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
