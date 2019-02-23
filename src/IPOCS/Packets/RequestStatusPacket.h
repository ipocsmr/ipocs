
#ifndef IPOCS_REQUESTSTATUSPACKET_H
#define IPOCS_REQUESTSTATUSPACKET_H

#include <stdint.h>
#include <Arduino.h>
#include "../Packet.h"

namespace IPOCS {

class RequestStatusPacket: public Packet {
  public:
    static Packet* create();
  protected:
    virtual uint8_t parseSpecific(uint8_t buffer[]);
    virtual uint8_t serializeSpecific(uint8_t buffer[]);
  private:
    RequestStatusPacket();
};

}
#endif
