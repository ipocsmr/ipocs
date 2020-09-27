#include "../LedControl.h"
#include <Arduino.h>

#define LED_PIN LED_BUILTIN

LedControl& LedControl::instance() {
    static LedControl ctrl;
    return ctrl;
}

void LedControl::begin() {
    this->m_controlState = OFF;
    this->m_ledState = ON;
    this->m_ulLastLoop = millis();
    this->m_ulKeepStateFor = 100;
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);
}

void LedControl::loop() {
  unsigned long ulCurrentTime = millis();
  if (ulCurrentTime - this->m_ulLastLoop >= this->m_ulKeepStateFor) {
    enum LedState newState = OFF; // = this->m_ledState == OFF ? ON : OFF;
    switch (this->m_controlState) {
      case ON:
        newState = ON;
        break;
      case OFF:
        newState = OFF;
        break;
      case BLINK:
        if (this->m_ledState == OFF) {
          newState = ON;
        } else {
          newState = OFF;
        }
        break;
    }
    digitalWrite(LED_PIN, newState == ON ? HIGH : LOW);
    this->m_ledState = newState;
    this->m_ulLastLoop = ulCurrentTime;
  }
}

void LedControl::setState(enum LedState newState, uint16_t interval) {
  this->m_controlState = newState;
  this->m_ulKeepStateFor = interval;
}
