
#include "SignalHeadWS2812.h"
#include "../BasicObject.h"
#include "../ObjectStore.h"
#include <FastLED.h>

#define MAX_SIGNAL_HEADS 60
CRGB lampHeads[MAX_SIGNAL_HEADS];
uint8_t lampsInUse = 0;

SignalHead::SignalHead() {
    this->led = &lampHeads[lampsInUse++];
    *(this->led) = CRGB::Red;
    // Dim a color by 25% (64/256ths)
    // using "video" scaling, meaning: never fading to full black
    this->led->fadeLightBy(64);
    // Reduce color to 75% (192/256ths) of its previous value
    // using "video" scaling, meaning: never fading to full black
    *(this->led) %= 192;
}

void SignalHead::handleOrder(IPOCS::Packet* basePacket) {
  /*
  if (basePacket->RNID_PACKET == 10) {
    if (this->frogOutput != 0) {
      digitalWrite(this->frogOutput, LOW);
    }
    for (PointsMotorNode* currentNode = this->first; currentNode != NULL; currentNode = currentNode->next)
    {
      currentNode->motor->handleOrder(basePacket);
    }
  } else if (basePacket->RNID_PACKET == 7) {
    this->sendStatus();
  } else {
    // TODO: Send alarm or something about invalid packet type.
  }
  */
}

IPOCS::Packet* SignalHead::getStatusPacket() {
  //IPOCS::PointsStatusPacket* pkt = (IPOCS::PointsStatusPacket*)IPOCS::PointsStatusPacket::create();
  //pkt->RQ_POINTS_STATE = this->lastSentState;
  return NULL;
}

void SignalHead::loop() {
}

void SignalHead::objectInit(const uint8_t configData[], int configDataLen) {
    this->led = &lampHeads[configData[0]];
}

static BasicObject* createSignalHead()
{
  return new SignalHead();
}

__attribute__((constructor))
static void initialize_signal_head() {
  ObjectStore::getInstance().registerType(25, &createSignalHead);
}
