#include "ConnectionRequestPacket.h"
#include <stddef.h>
#include <string.h>

IPOCS::ConnectionRequestPacket::ConnectionRequestPacket()
{
  this->RNID_PACKET = 1;
  this->RM_PROTOCOL_VERSION = 0;
  this->RXID_SITE_DATA_VERSION = NULL;
}

IPOCS::Packet* IPOCS::ConnectionRequestPacket::create()
{
  return new IPOCS::ConnectionRequestPacket();
}

uint8_t IPOCS::ConnectionRequestPacket::parseSpecific(uint8_t buffer[])
{
  this->RM_PROTOCOL_VERSION = (buffer[0] << 8) + buffer[1];
  size_t nameLength = strlen((const char* const)(buffer + 1));
  this->RXID_SITE_DATA_VERSION = new char[nameLength + 1];
  strcpy(this->RXID_SITE_DATA_VERSION, (const char* const)(buffer + 1));

  return 2 + nameLength + 1;
}

uint8_t IPOCS::ConnectionRequestPacket::serializeSpecific(uint8_t buffer[])
{
  buffer[0] = this->RM_PROTOCOL_VERSION >> 8;
  buffer[1] = this->RM_PROTOCOL_VERSION & 0xFF;
  strcpy((char*)(&buffer[2]), this->RXID_SITE_DATA_VERSION);
  return 2 + strlen(this->RXID_SITE_DATA_VERSION) + 1;
}

__attribute__((constructor))
static void initialize_packet_connectionrequest() {
  IPOCS::Packet::registerCreator(1, &IPOCS::ConnectionRequestPacket::create);
}
