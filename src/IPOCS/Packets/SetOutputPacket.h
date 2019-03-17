#ifndef IPOCS_SETOUTPUTPACKET_H
#define IPOCS_SETOUTPUTPACKET_H

#include <stdint.h>
#include "../Packet.h"

namespace IPOCS {
  class SetOutputPacket: public Packet {
    public:
      enum E_RQ_OUTPUT_COMMAND {
        ON = 1,
        OFF = 2
      };
      E_RQ_OUTPUT_COMMAND RQ_OUTPUT_COMMAND;
      uint16_t RT_DURATION;

      static Packet* create();
    protected:
      virtual uint8_t parseSpecific(uint8_t buffer[]);
      virtual uint8_t serializeSpecific(uint8_t buffer[]);
    private:
      SetOutputPacket();
  };
}
#endif
