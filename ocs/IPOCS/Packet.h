
#ifndef IPOCS_PACKET_H
#define IPOCS_PACKET_H

#include <stdint.h>

namespace IPOCS {

class Packet;
typedef IPOCS::Packet* (*PacketParserFun)();

class Packet {
  public:
    uint8_t RNID_PACKET;
    uint8_t RL_PACKET;
    uint8_t RM_ACK;

    static void registerCreator(uint8_t packetId, PacketParserFun function);
    static uint8_t create(Packet** pkt, uint8_t buffer[]);
    uint8_t serialize(uint8_t buffer[]);

    virtual ~Packet() {}
  protected:
    virtual uint8_t parseSpecific(uint8_t buffer[]) = 0;
    virtual uint8_t serializeSpecific(uint8_t buffer[]) = 0;
    Packet();
};

}
#endif
