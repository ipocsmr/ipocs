#ifndef LEDCONTROL_H
#define LEDCONTROL_H

enum LedState {
  OFF,
  ON,
  BLINK_MS100,
  BLINK_MS500
};

namespace esp {
    class LedControl {
        public:
            static LedControl& instance();
            void setState(enum LedState newState);

            void begin();
            void loop();
        private:
            unsigned long m_ulLastLoop = 0;
            unsigned long m_ulKeepStateFor = 0;
            enum LedState m_ledState = OFF;
            enum LedState m_controlState = OFF;
            LedControl() {}
    };
};

#endif