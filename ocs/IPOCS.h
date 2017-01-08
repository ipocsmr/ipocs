
#ifndef IPOCS_H
#define IPOCS_H

#include <stdint.h>
#include <Arduino.h>

namespace IPOCS {

class Packet {
  public:
    uint8_t RNID_PACKET;
    uint8_t RL_PACKET;
    uint8_t RM_ACK;

    static uint8_t create(Packet** pkt, uint8_t buffer[]);
    uint8_t serialize(uint8_t buffer[]);
  protected:
    virtual uint8_t parseSpecific(uint8_t buffer[]) = 0;
    virtual uint8_t serializeSpecific(uint8_t buffer[]) = 0;
    Packet();
};

class Message {
  public:
    class PacketNode {
      public:
        PacketNode* next;
        Packet* packet;
    };

    uint8_t RL_MESSAGE;
    String RXID_OBJECT;
    uint8_t numPackets;

    static Message* create();
    static Message* create(uint8_t buffer[]);
    uint8_t serialize(uint8_t buffer[]);
    void destroy();
    
    void addPacket(Packet* pkt);
    PacketNode* firstPacket;
  private:
    
    Message();

    PacketNode* lastPacket;
};

class ConnectionRequestPacket: public Packet {
  public:
    uint16_t RM_PROTOCOL_VERSION;
    String RXID_SITE_DATA_VERSION;

    static Packet* create();
  protected:
    virtual uint8_t parseSpecific(uint8_t buffer[]);
    virtual uint8_t serializeSpecific(uint8_t buffer[]);
  private:
    ConnectionRequestPacket();
};

class ConnectionResponsePacket: public Packet {
  public:
    uint16_t RM_PROTOCOL_VERSION;

    static Packet* create();
  protected:
    virtual uint8_t parseSpecific(uint8_t buffer[]);
    virtual uint8_t serializeSpecific(uint8_t buffer[]);
  private:
    ConnectionResponsePacket();
};

class RequestStatusPacket: public Packet {
  public:
    static Packet* create();
  protected:
    virtual uint8_t parseSpecific(uint8_t buffer[]);
    virtual uint8_t serializeSpecific(uint8_t buffer[]);
  private:
    RequestStatusPacket();
};

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

class PointsStatusPacket: public Packet {
  public:
    uint8_t RQ_POINTS_STATE;
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
