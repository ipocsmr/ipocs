/*
  Connection to JMRI server

  This sketch connects to the JMRI server
  using an Arduino Wiznet Ethernet shield.

  Circuit:
   Ethernet shield attached to pins 10, 11, 12, 13

  created 8 Dec 200916
  by P Elestedt

*/
#include "EspConnection.h"
#include "ObjectStore.h"
#include "../IPC/Message.h"
#include "../IPOCS/Message.h"
#include "log.h"

#define MIN_LOOP_TIME 10

void setup(void) {
  ard::EspConnection::instance().begin();
  #ifdef HAVE_HWSERIAL3
  Serial.begin(115200);
  LOG("We have 3 serials. Use one as debug...");
  #endif
}

void loop() {
  int loopStart = millis();
  ard::EspConnection::instance().loop();
  ObjectStore::getInstance().loop();
  // Make sure we're not working too fast for Arduino
  int loopEnd = millis();
  if ((loopEnd - loopStart) < MIN_LOOP_TIME) {
    delay(MIN_LOOP_TIME - (loopEnd - loopStart));
  }
}