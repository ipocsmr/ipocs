#include "PointsStatusPacket.h"

IPOCS::PointsStatusPacket::PointsStatusPacket()
{
  this->RNID_PACKET = 17;
  this->RQ_POINTS_STATE = (IPOCS::PointsStatusPacket::E_RQ_POINTS_STATE)0;
  this->RQ_RELEASE_STATE = 2;
  this->RT_OPERATION = 0;
}

IPOCS::Packet* IPOCS::PointsStatusPacket::create()
{
  return new IPOCS::PointsStatusPacket();
}

uint8_t IPOCS::PointsStatusPacket::parseSpecific(uint8_t buffer[])
{
  this->RQ_POINTS_STATE = (IPOCS::PointsStatusPacket::E_RQ_POINTS_STATE)buffer[0];
  this->RQ_RELEASE_STATE = buffer[1];
  this->RT_OPERATION = (buffer[2] << 8) + buffer[3];
  return 4;
}

uint8_t IPOCS::PointsStatusPacket::serializeSpecific(uint8_t buffer[])
{
  buffer[0] = this->RQ_POINTS_STATE;
  buffer[1] = this->RQ_RELEASE_STATE;
  buffer[2] = this->RT_OPERATION >> 8;
  buffer[3] = this->RT_OPERATION & 0xFF;
  return 4;
}
