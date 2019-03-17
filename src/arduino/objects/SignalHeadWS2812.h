/*****************
 * Signal Head - definition file.
 *
 * This file defines how to operate the NeoPixel Signal Head
 *
 * @author Fredrik Elestedt
 * @date
 * @history see the git log of the origin repository.
 *
 */
#ifndef NEOPIXELSIGNALHEAD_H
#define NEOPIXELSIGNALHEAD_H

#include "../BasicObject.h"
#include "../../IPOCS/Packet.h"

struct CRGB;

class SignalHead: public BasicObject {
  public:
    SignalHead();
    virtual void handleOrder(IPOCS::Packet* basePacket);
    virtual void loop();
  protected:
    virtual void objectInit(const uint8_t configData[], int configDataLen);
    virtual IPOCS::Packet* getStatusPacket();
  private:
    CRGB* led;
};

#endif
