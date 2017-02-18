
#ifndef IPOCS_POINTSSTATUSPACKET_H
#define IPOCS_POINTSSTATUSPACKET_H

#include <stdint.h>
#include "../Packet.h"

namespace IPOCS {

class PointsStatusPacket: public Packet {
  public:
    enum E_RQ_POINTS_STATE {
      RIGHT = 1,
      LEFT = 2,
      MOVING = 3,
      OUT_OF_CONTROL = 4
    };
    E_RQ_POINTS_STATE RQ_POINTS_STATE;
    uint8_t RQ_RELEASE_STATE;
    uint16_t RT_OPERATION;

    static Packet* create();
  protected:
    virtual uint8_t parseSpecific(uint8_t buffer[]);
    virtual uint8_t serializeSpecific(uint8_t buffer[]);
  private:
    PointsStatusPacket();
};

}
#endif
