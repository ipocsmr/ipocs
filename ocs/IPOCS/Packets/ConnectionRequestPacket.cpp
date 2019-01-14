#include "ConnectionRequestPacket.h"

IPOCS::ConnectionRequestPacket::ConnectionRequestPacket()
{
  this->RNID_PACKET = 1;
  this->RM_PROTOCOL_VERSION = 0;
}

IPOCS::Packet* IPOCS::ConnectionRequestPacket::create()
{
  return new IPOCS::ConnectionRequestPacket();
}

uint8_t IPOCS::ConnectionRequestPacket::parseSpecific(uint8_t buffer[])
{
  this->RM_PROTOCOL_VERSION = (buffer[0] << 8) + buffer[1];
  for (uint8_t* firstChar = buffer + 2; *firstChar != 0x00; firstChar++)
  {
    this->RXID_SITE_DATA_VERSION += String((char)(*firstChar));
  }
  return 2 + this->RXID_SITE_DATA_VERSION.length() + 1;
}

uint8_t IPOCS::ConnectionRequestPacket::serializeSpecific(uint8_t buffer[])
{
  buffer[0] = this->RM_PROTOCOL_VERSION >> 8;
  buffer[1] = this->RM_PROTOCOL_VERSION & 0xFF;
  this->RXID_SITE_DATA_VERSION.getBytes(buffer + 2, this->RXID_SITE_DATA_VERSION.length() + 1);
  return 2 + this->RXID_SITE_DATA_VERSION.length() + 1;
}

__attribute__((constructor))
static void initialize_packet_connectionrequest() {
  IPOCS::Packet::registerCreator(1, &IPOCS::ConnectionRequestPacket::create);
}
