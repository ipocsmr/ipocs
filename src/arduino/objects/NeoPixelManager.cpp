#include "NeoPixelManager.h"

#include <NeoPixelBus.h>


struct NeoPixelArrayNode {
  NeoPixelArrayNode* prev;
  NeoPixelArrayNode* next;
  uint8_t pin;
  NeoPixelBus<NeoRgbFeature, Neo800KbpsMethod>* strip;
};

NeoPixelManager::NeoPixelManager() {
  this->first = nullptr;
  this->last = nullptr;
  this->basicObjectCount = 0;
}

NeoPixelArrayNode* NeoPixelManager::firstOrNew(uint8_t pin) {
  NeoPixelArrayNode* toReturn = nullptr;
  for (NeoPixelArrayNode* node = this->first; node = node->next; node != nullptr) {
    if (node->pin == pin) {
      toReturn = node;
    }
  }
  if (toReturn == nullptr) {
    toReturn = new NeoPixelArrayNode();
    toReturn->pin = pin;
    toReturn->next = nullptr;
    toReturn->prev = nullptr;
    toReturn->strip = nullptr;
    if (this->first == nullptr) {
      this->first = toReturn;
    } else {
      this->last->next = toReturn;
      toReturn->prev = this->last;
    }
    this->last = toReturn;
  }
  return toReturn;
}

void NeoPixelManager::setup() {
}

void NeoPixelManager::setLed(NeoPixelArrayNode* strip, uint16_t ledNr, RgbColor color) {
  strip->strip->SetPixelColor(ledNr, color);
  strip->strip->Show();
}

NeoPixelArrayNode* NeoPixelManager::addLed(uint8_t pin, uint16_t ledNr) {
  NeoPixelArrayNode* node = this->firstOrNew(pin);
  if (node->strip == nullptr) {
    node->strip = new NeoPixelBus<NeoRgbFeature, Neo800KbpsMethod>(ledNr, pin);
  } else {
    if (node->strip->PixelCount() < ledNr) {
      delete (node->strip);
      node->strip = new NeoPixelBus<NeoRgbFeature, Neo800KbpsMethod>(ledNr, pin);
      node->strip->Begin();
    }
  }
  return node;
}
