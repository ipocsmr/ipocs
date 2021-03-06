
#include "Points.h"
#include "../ObjectStore.h"
#include "motors/PointsMotor.h"
#include "motors/PointsMotorStore.h"
#include "../EspConnection.h"
#include "../../IPC/Message.h"
#include "../../IPOCS/Message.h"

Points::Points() {
  this->lastSentState = IPOCS::PointsStatusPacket::E_RQ_POINTS_STATE::OUT_OF_CONTROL;
  this->first = NULL;
}

void Points::handleOrder(IPOCS::Packet* basePacket)
{
  if (basePacket->RNID_PACKET == 10) {
    if (this->frogOutput != 0) {
      digitalWrite(this->frogOutput, LOW);
    }
    for (PointsMotorNode* currentNode = this->first; currentNode != NULL; currentNode = currentNode->next)
    {
      currentNode->motor->handleOrder(basePacket);
    }
  } else if (basePacket->RNID_PACKET == 7) {
    this->sendStatus();
  } else {
    // TODO: Send alarm or something about invalid packet type.
  }
}

IPOCS::Packet* Points::getStatusPacket() {
  IPOCS::PointsStatusPacket* pkt = (IPOCS::PointsStatusPacket*)IPOCS::PointsStatusPacket::create();
  pkt->RQ_POINTS_STATE = this->lastSentState;
  return pkt;
}

void Points::loop()
{
  for (PointsMotorNode* currentNode = this->first; currentNode != NULL; currentNode = currentNode->next)
  {
    currentNode->motor->loop();
  }
  // TODO: Don't react on status changes instantly.

  IPOCS::PointsStatusPacket::E_RQ_POINTS_STATE allState = this->first->motor->getState();
  bool allSame = true;
  for (PointsMotorNode* currentNode = this->first; currentNode != NULL && allSame; currentNode = currentNode->next)
  {
    allSame = (allState == currentNode->motor->getState());
  }
  if (allSame && this->lastSentState != allState) {
    if (this->frogOutput != 0 && allState == IPOCS::PointsStatusPacket::E_RQ_POINTS_STATE::RIGHT) {
      digitalWrite(this->frogOutput, HIGH);
    }
    this->lastSentState = allState;
    this->sendStatus();
  }
}

void Points::objectInit(const uint8_t configData[], int configDataLen)
{
  this->frogOutput = configData[0];
  if (this->frogOutput != 0) {
    pinMode(this->frogOutput, OUTPUT);
  }
  const uint8_t* configDataCurrent = &configData[1];
  for (int index = 1; index < configDataLen; )
  {
    ard::EspConnection::instance().log("> Point (" + String(objectName) + ")");
    initPointsMotorFunction fun = NULL;
    PointsMotorStore::getInstance().getFunction(configDataCurrent[0], &fun);
    PointsMotor* pointsMotor = fun();
    int consumed = pointsMotor->objectInit(&configDataCurrent[1]) + 1;
    index += consumed;
    configDataCurrent += consumed;

    PointsMotorNode* node = new PointsMotorNode();
    node->next = NULL;
    node->motor = pointsMotor;
    if (this->first != NULL) {
      node->next = this->first;
    }
    this->first = node;
  }
}

static BasicObject* createPoints()
{
  return new Points();
}

__attribute__((constructor))
static void initialize_point_t1() {
  ObjectStore::getInstance().registerType(1, &createPoints);
}
