/*****************
 * Generic Output Type - definition file.
 *
 * This file defines how to operate the Generic Output.
 *
 * @author Fredrik Elestedt
 * @date
 * @history see the git log of the origin repository.
 */
#ifndef OUTPUT_H
#define OUTPUT_H

#include <stdint.h>
#include "../BasicObject.h"
#include "../../IPOCS/Packets/OutputStatusPacket.h"

class Output: public BasicObject {
  public:
    Output();
    virtual void handleOrder(IPOCS::Packet* basePacket);
    virtual void loop();
  protected:
    virtual void objectInit(const uint8_t configData[], int configDataLen);
    virtual IPOCS::Packet* getStatusPacket();
  private:
    uint32_t lastOrderTime;
    uint16_t offDelay;
    uint8_t outputPin;
    IPOCS::OutputStatusPacket::E_RQ_OUTPUT_STATE lastSentState;
};

#endif
