#include "RequestStatusPacket.h"

IPOCS::RequestStatusPacket::RequestStatusPacket()
{
  this->RNID_PACKET = 7;
}

IPOCS::Packet* IPOCS::RequestStatusPacket::create()
{
  return new IPOCS::RequestStatusPacket();
}

uint8_t IPOCS::RequestStatusPacket::parseSpecific(uint8_t buffer[])
{
  return 0;
}

uint8_t IPOCS::RequestStatusPacket::serializeSpecific(uint8_t buffer[])
{
  return 0;
}

__attribute__((constructor))
static void initialize_packet_requeststatus() {
  IPOCS::Packet::registerCreator(7, &IPOCS::RequestStatusPacket::create);
}
