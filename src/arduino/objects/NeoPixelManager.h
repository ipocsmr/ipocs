/*****************
 * Neopixel led handler
 *
 * @author Fredrik Elestedt
 * @date
 * @history see the git log of the origin repository.
 *
 */
#ifndef NEOPIXELMANAGER_H
#define NEOPIXELMANAGER_H

#include <stdint.h>
struct RgbColor;
struct CRGB;
struct NeoPixelArrayNode;

class NeoPixelManager {
  public:
    static NeoPixelManager& getInstance()
    {
      static NeoPixelManager instance;
      return instance;
    }

    void setup();
    NeoPixelArrayNode* addLed(uint8_t pin, uint16_t ledNr);
    void setLed(NeoPixelArrayNode* strip, uint16_t ledNr, RgbColor color);
  private:
    struct NeoPixelArrayNode* firstOrNew(uint8_t pin);

    struct NeoPixelArrayNode* first;
    struct NeoPixelArrayNode* last;
    long basicObjectCount;
    NeoPixelManager();
    NeoPixelManager(NeoPixelManager const&); // Don't Implement
    void operator=(NeoPixelManager const&); // Don't implement
};

#endif

