
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

    virtual ~Packet() {}
  protected:
    virtual uint8_t parseSpecific(uint8_t buffer[]) = 0;
    virtual uint8_t serializeSpecific(uint8_t buffer[]) = 0;
    Packet();
};

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

class ApplicationDataPacket: public Packet {
  public:
    uint16_t RNID_XUSER;
    uint8_t data[100];

    static Packet* create();
  protected:
    virtual uint8_t parseSpecific(uint8_t buffer[]);
    virtual uint8_t serializeSpecific(uint8_t buffer[]) { return 0; }
  private:
    ApplicationDataPacket();
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
