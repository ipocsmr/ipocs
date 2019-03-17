#ifndef IPOCS_OUTPUTSTATUSPACKET_H
#define IPOCS_OUTPUTSTATUSPACKET_H

#include <stdint.h>
#include "../Packet.h"

namespace IPOCS {
  class OutputStatusPacket: public Packet {
    public:
      enum E_RQ_OUTPUT_STATE {
        ON = 1,
        OFF = 2
      };
      E_RQ_OUTPUT_STATE RQ_OUTPUT_STATE;

      static Packet* create();
    protected:
      virtual uint8_t parseSpecific(uint8_t buffer[]);
      virtual uint8_t serializeSpecific(uint8_t buffer[]);
    private:
      OutputStatusPacket();
  };
}
#endif
