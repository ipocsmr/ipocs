
#ifndef IPOCS_MESSAGE_H
#define IPOCS_MESSAGE_H

#include <stdint.h>
#include <Arduino.h>

namespace IPOCS {

class Packet;

class Message {
  public:
    uint8_t RL_MESSAGE;
    String RXID_OBJECT;

    ~Message();

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
