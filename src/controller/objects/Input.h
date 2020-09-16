/*****************
 * Generic Input Type - definition file.
 *
 * This file defines how to operate the Generic Input.
 *
 * @author Fredrik Elestedt
 * @date
 * @history see the git log of the origin repository.
 */
#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>
#include "../BasicObject.h"
#include "../../IPOCS/Packets/InputStatusPacket.h"

class Input: public BasicObject {
  public:
    Input();
    virtual void handleOrder(IPOCS::Packet* basePacket);
    virtual void loop();
  protected:
    virtual void objectInit(const uint8_t configData[], int configDataLen);
    virtual IPOCS::Packet* getStatusPacket();
  private:
    uint8_t inputPin;
    uint8_t debounceTime;
    uint8_t releaseHoldTime;
    uint16_t changeFirstRegisteredTime;
    IPOCS::InputStatusPacket::E_RQ_INPUT_STATE lastSentState;
};

#endif
