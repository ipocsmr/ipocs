
#ifndef IPOCS_THROWPOINTSPACKET_H
#define IPOCS_THROWPOINTSPACKET_H

#include <stdint.h>
#include "../Packet.h"

namespace IPOCS {


class ThrowPointsPacket: public Packet {
  public:
    enum E_RQ_POINTS_COMMAND {
      DIVERT_RIGHT = 1,
      DIVERT_LEFT = 2
    };
    E_RQ_POINTS_COMMAND RQ_POINTS_COMMAND;

    static Packet* create();
  protected:
    virtual uint8_t parseSpecific(uint8_t buffer[]);
    virtual uint8_t serializeSpecific(uint8_t buffer[]);
  private:
    ThrowPointsPacket();
};

}
#endif
