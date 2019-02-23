
#ifndef IPOCS_CONNECTIONRESPONSEPACKET_H
#define IPOCS_CONNECTIONRESPONSEPACKET_H

#include <stdint.h>
#include "../Packet.h"

namespace IPOCS {

class ConnectionResponsePacket: public Packet {
  public:
    uint16_t RM_PROTOCOL_VERSION;

    static Packet* create();
  protected:
    virtual uint8_t parseSpecific(uint8_t buffer[]);
    virtual uint8_t serializeSpecific(uint8_t buffer[]);
  private:
    ConnectionResponsePacket();
};

}
#endif
