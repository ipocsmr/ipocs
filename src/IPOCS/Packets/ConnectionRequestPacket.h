
#ifndef IPOCS_CONNECTIONREQESTPACKET_H
#define IPOCS_CONNECTIONREQESTPACKET_H

#include <stdint.h>
#include "../Packet.h"

namespace IPOCS {

class ConnectionRequestPacket: public Packet {
  public:
    uint16_t RM_PROTOCOL_VERSION;
    char* RXID_SITE_DATA_VERSION;

    static Packet* create();
  protected:
    virtual uint8_t parseSpecific(uint8_t buffer[]);
    virtual uint8_t serializeSpecific(uint8_t buffer[]);
  private:
    ConnectionRequestPacket();
};

}
#endif
