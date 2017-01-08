#include <SPI.h>

#include "ObjectStore.h"
#include "point_t1.h"
#include "ServerConnection.h"
#include "IPOCS.h"

const int normal = 20;
const int thrown = 150;
const int pointStepTime = 30;

PointT1::PointT1()
{
}

void PointT1::objectInit(byte configData[], int configDataLen)
{
  this->lastRun = millis();
  this->setPos = normal;
  this->curPos = normal + 1;
  if (configDataLen == 1)
  {
    Serial.println("Attaching point to pin " + String(configData[0]));
    this->object.attach(configData[0]);
  }
}

void PointT1::handleOrder(IPOCS::Packet* basePacket)
{
  if (basePacket->RNID_PACKET == 10)
  {
    IPOCS::ThrowPointsPacket* packet = (IPOCS::ThrowPointsPacket*)basePacket;
    switch (packet->RQ_POINTS_COMMAND)
    {
      case IPOCS::ThrowPointsPacket::DIVERT_LEFT: this->setPos = normal; break;
      case IPOCS::ThrowPointsPacket::DIVERT_RIGHT: this->setPos = thrown; break;
      default:
        // TODO: Send error about invalid value
        break;
    }
  } else if (basePacket->RNID_PACKET == 7) {
    IPOCS::Message* msg = IPOCS::Message::create();
    msg->RXID_OBJECT = this->objectName;

    IPOCS::PointsStatusPacket* pkt = (IPOCS::PointsStatusPacket*)IPOCS::PointsStatusPacket::create();
    switch (this->curPos) {
      case normal: pkt->RQ_POINTS_STATE = 2; break;
      case thrown: pkt->RQ_POINTS_STATE = 1; break;
      default: pkt->RQ_POINTS_STATE = 3; break;
    }
    msg->addPacket(pkt);

    ServerConnection::getInstance().send(msg);
    msg->destroy();
    delete msg;
  } else {
    // TODO: Send alarm or something about invalid packet type.
  }
}

void PointT1::update()
{
  if (millis() - this->lastRun > pointStepTime) {
    this->lastRun = millis();
    // For each object, handle it.
    if (this->setPos == this->curPos)
      return;
    if (this->curPos == normal || this->curPos == thrown)
    {
      // Send status about moving.
      IPOCS::Message* msg = IPOCS::Message::create();
      msg->RXID_OBJECT = this->objectName;

      IPOCS::PointsStatusPacket* pkt = (IPOCS::PointsStatusPacket*)IPOCS::PointsStatusPacket::create();
      pkt->RQ_POINTS_STATE = 3;
      msg->addPacket(pkt);

      ServerConnection::getInstance().send(msg);
      msg->destroy();
      delete msg;
    }
    // Else determine which direction to move.
    int direction = this->setPos > this->curPos ? +1 : -1;
    // And then move
    this->curPos += direction;
    this->object.write(this->curPos);
    // And possibly notify on reaching end position (obsolete if we have an input to look at)
    if (this->curPos == this->setPos)
    {
      IPOCS::Message* msg = IPOCS::Message::create();
      msg->RXID_OBJECT = this->objectName;

      IPOCS::PointsStatusPacket* pkt = (IPOCS::PointsStatusPacket*)IPOCS::PointsStatusPacket::create();
      pkt->RQ_POINTS_STATE = this->setPos == thrown ? 1 : 2;
      msg->addPacket(pkt);

      ServerConnection::getInstance().send(msg);
      msg->destroy();
      delete msg;
    }
  }
}

static BasicObject* createPointT1()
{
  return new PointT1();
}

__attribute__((constructor))
static void initialize_point_t1() {
  ObjectStore::getInstance().registerType(4, &createPointT1);
}
