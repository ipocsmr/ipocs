#include "ApplicationDataPacket.h"

#include <string.h>

IPOCS::ApplicationDataPacket::ApplicationDataPacket()
{
  this->RNID_PACKET = 5;
  this->RNID_XUSER = 0;
  memset(this->data, 0x00, 100);
}

IPOCS::Packet* IPOCS::ApplicationDataPacket::create()
{
  return new IPOCS::ApplicationDataPacket();
}

uint8_t IPOCS::ApplicationDataPacket::parseSpecific(uint8_t buffer[])
{
  this->RNID_XUSER = (buffer[0] << 8) + buffer[1];
  uint8_t remaining = this->RL_PACKET - 2 - 3;
  for (uint8_t i = 0; i < remaining; i++)
  {
    this->data[i] = buffer[2 + i];
  }
  return this->RL_PACKET - 3;
}

__attribute__((constructor))
static void initialize_packet_applicationdata() {
  IPOCS::Packet::registerCreator(5, &IPOCS::ApplicationDataPacket::create);
}
