#include "Message.h"
#include "Packet.h"
#include <stddef.h>
#include <string.h>

/*******************************************************************
   Message
*/
IPOCS::Message::Message()
{
  this->RL_MESSAGE = 0;
  this->packet = NULL;
  this->RXID_OBJECT = NULL;
}

IPOCS::Message* IPOCS::Message::create()
{
  return new IPOCS::Message();
}

IPOCS::Message* IPOCS::Message::create(uint8_t buffer[])
{
  IPOCS::Message* msg = new IPOCS::Message();
  msg->RL_MESSAGE = buffer[0];

  size_t nameLength = strlen((const char* const)(buffer + 1));
  msg->RXID_OBJECT = new char[nameLength + 1];
  strcpy(msg->RXID_OBJECT, (const char* const)(buffer + 1));
  // 1 byte for message length + object name + null byte
  uint8_t msgParsed = 1 + nameLength + 1;
  // Use pointer arithmetics to read all packets in buffer.
  uint8_t* pktBuf = buffer + msgParsed;
  Packet* pkt = NULL;
  uint8_t readBytes = IPOCS::Packet::create(&pkt, pktBuf);
  msg->setPacket(pkt);
  pktBuf += readBytes;
  return msg;
}

void IPOCS::Message::setObject(const char* const RXID_OBJECT)
{
  if (this->RXID_OBJECT != NULL) {
    delete this->RXID_OBJECT;
    this->RXID_OBJECT = NULL;
  }
  this->RXID_OBJECT = new char[strlen(RXID_OBJECT) + 1];
  strcpy(this->RXID_OBJECT, RXID_OBJECT);
}

uint8_t IPOCS::Message::serialize(uint8_t buffer[])
{
  this->RL_MESSAGE = 1 + strlen(this->RXID_OBJECT) + 1;
  memcpy(&buffer[1], this->RXID_OBJECT, strlen(this->RXID_OBJECT) + 1);
  this->RL_MESSAGE += this->packet->serialize(buffer + this->RL_MESSAGE);
  buffer[0] = this->RL_MESSAGE;
  return this->RL_MESSAGE;
}

void IPOCS::Message::setPacket(Packet* pkt)
{
  this->packet = pkt;
}

IPOCS::Message::~Message()
{
  delete (this->packet);
  if (this->RXID_OBJECT != NULL) {
    delete this->RXID_OBJECT;
    this->RXID_OBJECT = NULL;
  }
}
