#include "ThrowPointsPacket.h"

IPOCS::ThrowPointsPacket::ThrowPointsPacket()
{
  this->RNID_PACKET = 10;
}

IPOCS::Packet* IPOCS::ThrowPointsPacket::create()
{
  return new IPOCS::ThrowPointsPacket();
}

uint8_t IPOCS::ThrowPointsPacket::parseSpecific(uint8_t buffer[])
{
  this->RQ_POINTS_COMMAND = (ThrowPointsPacket::E_RQ_POINTS_COMMAND)buffer[0];
  return 1;
}

uint8_t IPOCS::ThrowPointsPacket::serializeSpecific(uint8_t buffer[])
{
  buffer[0] = this->RQ_POINTS_COMMAND;
  return 1;
}

__attribute__((constructor))
static void initialize_packet_throwpoints() {
  IPOCS::Packet::registerCreator(10, &IPOCS::ThrowPointsPacket::create);
}
