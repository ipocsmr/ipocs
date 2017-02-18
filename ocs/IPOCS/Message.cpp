#include "Message.h"
#include "Packet.h"

/*******************************************************************
   Message
*/
IPOCS::Message::Message()
{
  this->RL_MESSAGE = 0;
  this->packet = NULL;
}

IPOCS::Message* IPOCS::Message::create()
{
  return new IPOCS::Message();
}

IPOCS::Message* IPOCS::Message::create(uint8_t buffer[])
{
  IPOCS::Message* msg = new IPOCS::Message();
  msg->RL_MESSAGE = buffer[0];

  for (uint8_t* firstChar = buffer + 1; *firstChar != 0x00; firstChar++)
  {
    msg->RXID_OBJECT += String((char)(*firstChar));
  }
  // 1 byte for message length + object name + null byte
  uint8_t msgParsed = 1 + msg->RXID_OBJECT.length() + 1;
  // Use pointer arithmetics to read all packets in buffer.
  uint8_t* pktBuf = buffer + msgParsed;
  Packet* pkt = NULL;
  uint8_t readBytes = IPOCS::Packet::create(&pkt, pktBuf);
  msg->setPacket(pkt);
  pktBuf += readBytes;
  return msg;
}

uint8_t IPOCS::Message::serialize(uint8_t buffer[])
{
  this->RL_MESSAGE = 1 + this->RXID_OBJECT.length() + 1;
  this->RXID_OBJECT.getBytes(buffer + 1, this->RXID_OBJECT.length() + 1);
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
}
