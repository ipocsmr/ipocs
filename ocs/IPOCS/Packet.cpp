#include "Packet.h"
#include "Arduino.h"

class PacketParserNode {
  public:
    PacketParserNode* next;
    IPOCS::PacketParserFun func;
    uint8_t packetId;

    PacketParserNode(uint8_t packetId, IPOCS::PacketParserFun func):
      next(NULL), func(func), packetId(packetId) {}
};

PacketParserNode* firstNode = NULL;

void IPOCS::Packet::registerCreator(uint8_t packetId, IPOCS::PacketParserFun function)
{
  PacketParserNode* newNode = new PacketParserNode(packetId, function);
  if (firstNode != NULL)
  {
    newNode->next = firstNode;
  }
  firstNode = newNode;
}

IPOCS::PacketParserFun getCreator(uint8_t packetId)
{
  IPOCS::PacketParserFun toReturn = NULL;
  for (PacketParserNode* node = firstNode; node != NULL; node = node->next)
  {
    if (node->packetId == packetId)
    {
      toReturn = node->func;
    }
  }
  return toReturn;
}

IPOCS::Packet::Packet()
{
  this->RNID_PACKET = 0;
  this->RL_PACKET = 0;
  this->RM_ACK = 0;
}

uint8_t IPOCS::Packet::create(Packet** pkt, uint8_t buffer[])
{
  *pkt = getCreator(buffer[0])();
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
