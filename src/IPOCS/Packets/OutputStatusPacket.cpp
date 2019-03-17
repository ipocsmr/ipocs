#include "OutputStatusPacket.h"

IPOCS::OutputStatusPacket::OutputStatusPacket()
{
  this->RNID_PACKET = 22;
}

IPOCS::Packet* IPOCS::OutputStatusPacket::create()
{
  return new IPOCS::OutputStatusPacket();
}

uint8_t IPOCS::OutputStatusPacket::parseSpecific(uint8_t buffer[])
{
  this->RQ_OUTPUT_STATE = (OutputStatusPacket::E_RQ_OUTPUT_STATE)buffer[0];
  return 1;
}

uint8_t IPOCS::OutputStatusPacket::serializeSpecific(uint8_t buffer[])
{
  buffer[0] = this->RQ_OUTPUT_STATE;
  return 1;
}

__attribute__((constructor))
static void initialize_packet_outputstatus() {
  IPOCS::Packet::registerCreator(22, &IPOCS::OutputStatusPacket::create);
}
