#include "SetOutputPacket.h"

IPOCS::SetOutputPacket::SetOutputPacket()
{
  this->RNID_PACKET = 13;
}

IPOCS::Packet* IPOCS::SetOutputPacket::create()
{
  return new IPOCS::SetOutputPacket();
}

uint8_t IPOCS::SetOutputPacket::parseSpecific(uint8_t buffer[])
{
  this->RQ_OUTPUT_COMMAND = (SetOutputPacket::E_RQ_OUTPUT_COMMAND)buffer[0];
  this->RT_DURATION = (buffer[1] << 8) + buffer[2];
  return 3;
}

uint8_t IPOCS::SetOutputPacket::serializeSpecific(uint8_t buffer[])
{
  buffer[0] = this->RQ_OUTPUT_COMMAND;
  buffer[1] = this->RT_DURATION >> 8;
  buffer[2] = this->RT_DURATION & 0xFF;
  return 3;
}

__attribute__((constructor))
static void initialize_packet_setoutput() {
  IPOCS::Packet::registerCreator(13, &IPOCS::SetOutputPacket::create);
}
