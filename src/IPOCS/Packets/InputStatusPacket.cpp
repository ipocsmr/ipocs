#include "InputStatusPacket.h"

IPOCS::InputStatusPacket::InputStatusPacket()
{
  this->RNID_PACKET = 20;
}

IPOCS::Packet* IPOCS::InputStatusPacket::create()
{
  return new IPOCS::InputStatusPacket();
}

uint8_t IPOCS::InputStatusPacket::parseSpecific(uint8_t buffer[])
{
  this->RQ_INPUT_STATE = (InputStatusPacket::E_RQ_INPUT_STATE)buffer[0];
  return 1;
}

uint8_t IPOCS::InputStatusPacket::serializeSpecific(uint8_t buffer[])
{
  buffer[0] = this->RQ_INPUT_STATE;
  return 1;
}

__attribute__((constructor))
static void initialize_packet_inputstatus() {
  IPOCS::Packet::registerCreator(20, &IPOCS::InputStatusPacket::create);
}
