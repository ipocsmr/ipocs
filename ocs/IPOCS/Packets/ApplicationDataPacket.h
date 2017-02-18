
#ifndef IPOCS_APPLICATIONDATAPACKET_H
#define IPOCS_APPLICATIONDATAPACKET_H

#include <stdint.h>
#include "../Packet.h"

namespace IPOCS {

class ApplicationDataPacket: public Packet {
  public:
    uint16_t RNID_XUSER;
    uint8_t data[100];

    static Packet* create();
  protected:
    virtual uint8_t parseSpecific(uint8_t buffer[]);
    virtual uint8_t serializeSpecific(uint8_t buffer[]) { return 0; }
  private:
    ApplicationDataPacket();
};

}
#endif
