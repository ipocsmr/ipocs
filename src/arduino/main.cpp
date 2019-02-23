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

#ifdef HAVE_HWSERIAL3
#define PRINT_TIME 50
unsigned long lastPrint = 0;
#endif

void setup(void) {
  ard::EspConnection::instance().begin();
  #ifdef HAVE_HWSERIAL3
  Serial.begin(115200);
  Serial.print('r');
  Serial.flush();
  lastPrint = millis();
  #endif
}

void loop() {
  ard::EspConnection::instance().loop();
  ObjectStore::getInstance().loop();
#ifdef HAVE_HWSERIAL3
  // Make sure we're not working too fast for Arduino
  unsigned long now = millis();
  if ((now - lastPrint) >= PRINT_TIME) {
    lastPrint = now;
    Serial.print('.');
    Serial.flush();
  }
#endif
}