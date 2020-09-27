#ifndef LEDCONTROL_H
#define LEDCONTROL_H

#include <stdint.h>

enum LedState {
  OFF,
  ON,
  BLINK
};

class LedControl {
    public:
        static LedControl& instance();
        void setState(enum LedState newState, uint16_t interval = 100);

        void begin();
        void loop();
    private:
        unsigned long m_ulLastLoop = 0;
        unsigned long m_ulKeepStateFor = 0;
        enum LedState m_ledState = OFF;
        enum LedState m_controlState = OFF;
        LedControl() {}
};

#endif