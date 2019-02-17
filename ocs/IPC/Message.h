
#ifndef IPC_MESSAGE_H
#define IPC_MESSAGE_H

#include <stdint.h>

namespace IPC {

  enum RT_TYPE {
    STARTED = 0x00,
    SITEDATA = 0x01,
    RESTART = 0x02,
    IPOCS = 0x10,
    IPING = 0x20,
    IPONG = 0x21,
    ILOG = 0x30
  };

  class Message {
    public:
      uint8_t RL_MESSAGE;
      uint8_t RT_TYPE;

      ~Message();

      static Message* create();

      /**
       * Deserialize message.
       * 
       * Note! Will not verify CRC
       **/
      static Message* create(const uint8_t buffer[]);
      uint8_t serialize(uint8_t buffer[]);
      /**
       * Verify checksum in buffer;
       **/
      static bool verifyChecksum(const uint8_t buffer[]);

      void setPayload();
      void setPayload(const uint8_t* buffer, const uint8_t length);
      uint8_t* pld;

    private:
      Message();
  };

}
#endif
