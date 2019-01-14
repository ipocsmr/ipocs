#include <SPI.h>

#include "PointsMotorStore.h"
#include "PointsMotor_Servo.h"
#include "../../ServerConnection.h"
#include "../../IPOCS/Packets/ThrowPointsPacket.h"

const int servoRightVal = 20;
const int servoLeftVal = 150;
const int pointStepTime = 20;
// Left state is less than or equal to this value.
const int StateLeft = 399;
// Moving/Out Of Control state is less than or equal to this value (Out Of Control after 10s)
const int StateMoving = 799;
// Right state is less than or equal to this value.
const int StateRight = 1023;

PointsMotor_Servo::PointsMotor_Servo()
{
}

int PointsMotor_Servo::objectInit(byte configData[])
{
  this->lastRun = millis();
  this->setPos = servoRightVal;
  this->curPos = servoRightVal;

  this->object.attach(configData[1]);
  this->posInput = configData[2];
#ifdef HAVE_HWSERIAL3
  Serial.println("  -> " + String(configData[1]) + " : "+ String(configData[2]));
  Serial.flush();
#endif
  return 3;
}

void PointsMotor_Servo::handleOrder(IPOCS::Packet* basePacket)
{
  if (basePacket->RNID_PACKET == 10)
  {
    IPOCS::ThrowPointsPacket* packet = (IPOCS::ThrowPointsPacket*)basePacket;
    switch (packet->RQ_POINTS_COMMAND)
    {
      case IPOCS::ThrowPointsPacket::DIVERT_RIGHT: this->setPos = servoRightVal; break;
      case IPOCS::ThrowPointsPacket::DIVERT_LEFT: this->setPos = servoLeftVal; break;
      default:
        // TODO: Send error about invalid value
        break;
    }
  }
}

void PointsMotor_Servo::loop()
{
  if (millis() - this->lastRun > pointStepTime) {
    this->lastRun = millis();
    // For each object, handle it.
    if (this->setPos == this->curPos)
      return;
    // Else determine which direction to move.
    int direction = this->setPos > this->curPos ? +1 : -1;
    // And then move
    this->curPos += direction;
    this->object.write(this->curPos);

    // And possibly notify on reaching end position (obsolete if we have an input to look at)
  }
}

IPOCS::PointsStatusPacket::E_RQ_POINTS_STATE PointsMotor_Servo::getState()
{
  IPOCS::PointsStatusPacket::E_RQ_POINTS_STATE pos = IPOCS::PointsStatusPacket::E_RQ_POINTS_STATE::OUT_OF_CONTROL;
  if (this->posInput == 0) {
    if (this->curPos == this->setPos) {
      pos = this->setPos == servoLeftVal ? IPOCS::PointsStatusPacket::E_RQ_POINTS_STATE::LEFT : IPOCS::PointsStatusPacket::E_RQ_POINTS_STATE::RIGHT;
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

static PointsMotor* createPointsMotor_Servo()
{
  return new PointsMotor_Servo();
}

__attribute__((constructor))
static void initialize_pointsmotor_servo() {
  PointsMotorStore::getInstance().registerType(2, &createPointsMotor_Servo);
}
