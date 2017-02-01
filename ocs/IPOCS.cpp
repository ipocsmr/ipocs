#include "IPOCS.h"

typedef IPOCS::Packet* (*PacketParserFun)();
const PacketParserFun packetParsers[] = {
  IPOCS::ConnectionRequestPacket::create,
  IPOCS::ConnectionResponsePacket::create,
  NULL,
  NULL,
  NULL,
  NULL,
  IPOCS::RequestStatusPacket::create,
  NULL,
  NULL,
  IPOCS::ThrowPointsPacket::create
};

/*******************************************************************
   Message
*/
IPOCS::Message::Message()
{
  this->RL_MESSAGE = 0;
  this->firstPacket = NULL;
  this->lastPacket = NULL;
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
  for (uint8_t* pktBuf = buffer + msgParsed; pktBuf < (buffer + msg->RL_MESSAGE); ) {
    Packet* pkt = NULL;
    uint8_t readBytes = IPOCS::Packet::create(&pkt, pktBuf);
    msg->addPacket(pkt);
    pktBuf += readBytes;
  }
  return msg;
}

uint8_t IPOCS::Message::serialize(uint8_t buffer[])
{
  this->RL_MESSAGE = 1 + this->RXID_OBJECT.length() + 1;
  this->RXID_OBJECT.getBytes(buffer + 1, this->RXID_OBJECT.length() + 1);

  for (PacketNode* pktN = this->firstPacket; pktN != NULL; pktN = pktN->next)
  {
    this->RL_MESSAGE += pktN->packet->serialize(buffer + this->RL_MESSAGE);
  }
  buffer[0] = this->RL_MESSAGE;
  return this->RL_MESSAGE;
}

void IPOCS::Message::addPacket(Packet* pkt)
{
  PacketNode* newNode = new PacketNode();
  newNode->next = NULL;
  newNode->packet = pkt;
  if (this->firstPacket == NULL)
  {
    this->firstPacket = newNode;
  } else {
    this->lastPacket->next = newNode;
  }
  this->lastPacket = newNode;
  this->numPackets++;
}

void IPOCS::Message::destroy()
{
  for (PacketNode* node = this->firstPacket; node != NULL; )
  {
    PacketNode* next = node->next;
    delete node->packet;
    delete node;
    node = next;
  }
}

/*******************************************************************
 *  Packet
 */
IPOCS::Packet::Packet()
{
  this->RNID_PACKET = 0;
  this->RL_PACKET = 0;
  this->RM_ACK = 0;
}

uint8_t IPOCS::Packet::create(Packet** pkt, uint8_t buffer[])
{
  *pkt = packetParsers[buffer[0] - 1]();
  (*pkt)->RNID_PACKET = buffer[0];
  (*pkt)->RL_PACKET = buffer[1];
  (*pkt)->RM_ACK = buffer[2];
  return 3 + (*pkt)->parseSpecific(buffer + 3);
}

uint8_t IPOCS::Packet::serialize(uint8_t buffer[])
{
  buffer[0] = this->RNID_PACKET;
  this->RL_PACKET = 3 + this->serializeSpecific(buffer + 3);
  buffer[1] = this->RL_PACKET;
  buffer[2] = this->RM_ACK;
  return this->RL_PACKET;
}

/*******************************************************************
 *  ConnectionRequestPacket
 */
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

/*******************************************************************
 *  ConnectionResponsePacket
 */
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

/*******************************************************************
 *  RequestStatusPacket
 */
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

/*******************************************************************
 *  ThrowPointPacket
 */
IPOCS::ThrowPointsPacket::ThrowPointsPacket()
{
  this->RNID_PACKET = 10;
}

IPOCS::Packet* IPOCS::ThrowPointsPacket::create()
{
  return new IPOCS::ThrowPointsPacket();
}

uint8_t IPOCS::ThrowPointsPacket::parseSpecific(uint8_t buffer[])
{
  this->RQ_POINTS_COMMAND = (ThrowPointsPacket::E_RQ_POINTS_COMMAND)buffer[0];
  return 1;
}

uint8_t IPOCS::ThrowPointsPacket::serializeSpecific(uint8_t buffer[])
{
  buffer[0] = this->RQ_POINTS_COMMAND;
  return 1;
}

/*******************************************************************
 *  PointsStatusPacket
 */
IPOCS::PointsStatusPacket::PointsStatusPacket()
{
  this->RNID_PACKET = 17;
  this->RQ_POINTS_STATE = (IPOCS::PointsStatusPacket::E_RQ_POINTS_STATE)0;
  this->RQ_RELEASE_STATE = 2;
  this->RT_OPERATION = 0;
}

IPOCS::Packet* IPOCS::PointsStatusPacket::create()
{
  return new IPOCS::PointsStatusPacket();
}

uint8_t IPOCS::PointsStatusPacket::parseSpecific(uint8_t buffer[])
{
  this->RQ_POINTS_STATE = (IPOCS::PointsStatusPacket::E_RQ_POINTS_STATE)buffer[0];
  this->RQ_RELEASE_STATE = buffer[1];
  this->RT_OPERATION = (buffer[2] << 8) + buffer[3];
  return 4;
}

uint8_t IPOCS::PointsStatusPacket::serializeSpecific(uint8_t buffer[])
{
  buffer[0] = this->RQ_POINTS_STATE;
  buffer[1] = this->RQ_RELEASE_STATE;
  buffer[2] = this->RT_OPERATION >> 8;
  buffer[3] = this->RT_OPERATION & 0xFF;
  return 4;
}

