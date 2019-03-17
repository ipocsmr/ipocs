#ifndef IPOCS_INPUTSTATUSPACKET_H
#define IPOCS_INPUTSTATUSPACKET_H

#include <stdint.h>
#include "../Packet.h"

namespace IPOCS {
  class InputStatusPacket: public Packet {
    public:
      enum E_RQ_INPUT_STATE {
        ON = 1,
        OFF = 2
      };
      E_RQ_INPUT_STATE RQ_INPUT_STATE;

      static Packet* create();
    protected:
      virtual uint8_t parseSpecific(uint8_t buffer[]);
      virtual uint8_t serializeSpecific(uint8_t buffer[]);
    private:
      InputStatusPacket();
  };
}
#endif
