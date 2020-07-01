#include "LedControl.hpp"
#include <Arduino.h>

#define LED_PIN 14

esp::LedControl& esp::LedControl::instance() {
    static esp::LedControl ctrl;
    return ctrl;
}

void esp::LedControl::begin() {
    this->m_controlState = OFF;
    this->m_ledState = ON;
    this->m_ulLastLoop = millis();
    this->m_ulKeepStateFor = 100;
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);
}

void esp::LedControl::loop() {
  unsigned long ulCurrentTime = millis();
  if (ulCurrentTime - this->m_ulLastLoop >= this->m_ulKeepStateFor) {
    enum LedState newState = OFF; // = this->m_ledState == OFF ? ON : OFF;
    this->m_ulKeepStateFor = 100;
    switch (this->m_controlState) {
      case ON:
        newState = ON;
        break;
      case OFF:
        newState = OFF;
        break;
      case BLINK_MS100:
        if (this->m_ledState == OFF) {
          newState = ON;
        } else {
          newState = OFF;
        }
        break;
      case BLINK_MS500:
        if (this->m_ledState == OFF) {
          newState = ON;
        } else {
          newState = OFF;
        }
        this->m_ulKeepStateFor = 500;
        break;
    }
    digitalWrite(LED_PIN, newState == ON ? HIGH : LOW);
    this->m_ledState = newState;
    this->m_ulLastLoop = ulCurrentTime;
  }
}

void esp::LedControl::setState(enum LedState newState) {
  this->m_controlState = newState;
}
