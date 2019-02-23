
#ifndef IPOCS_MESSAGE_H
#define IPOCS_MESSAGE_H

#include <stdint.h>

namespace IPOCS {

class Packet;

class Message {
  public:
    uint8_t RL_MESSAGE;
  private:
    char* RXID_OBJECT;
  public:
    ~Message();

    void setObject(const char* const RXID_OBJECT);
    const char* const getObject() { return RXID_OBJECT; }

    static Message* create();
    static Message* create(uint8_t buffer[]);
    uint8_t serialize(uint8_t buffer[]);

    void setPacket(Packet* pkt);
    Packet* packet;

  private:
    Message();
};

}
#endif
