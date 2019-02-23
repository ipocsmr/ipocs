#include "ConnectionResponsePacket.h"

IPOCS::ConnectionResponsePacket::ConnectionResponsePacket()
{
  this->RNID_PACKET = 2;
  this->RM_PROTOCOL_VERSION = 0;
}

IPOCS::Packet* IPOCS::ConnectionResponsePacket::create()
{
  return new IPOCS::ConnectionResponsePacket();
}

uint8_t IPOCS::ConnectionResponsePacket::parseSpecific(uint8_t buffer[])
{
  this->RM_PROTOCOL_VERSION = (buffer[0] << 8) + buffer[1];
  return 2;
}

uint8_t IPOCS::ConnectionResponsePacket::serializeSpecific(uint8_t buffer[])
{
  buffer[0] = this->RM_PROTOCOL_VERSION >> 8;
  buffer[1] = this->RM_PROTOCOL_VERSION & 0xFF;
  return 2;
}

__attribute__((constructor))
static void initialize_packet_connectionresponse() {
  IPOCS::Packet::registerCreator(2, &IPOCS::ConnectionResponsePacket::create);
}
