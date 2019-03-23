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
#include <avr/wdt.h>

#ifdef HAVE_HWSERIAL1
#define PRINT_TIME 50
unsigned long lastPrint = 0;
#endif

void setup(void) {
  MCUSR = ~(1 << WDRF); // allow us to disable WD
  wdt_disable();
  ard::EspConnection::instance().begin();
  #ifdef HAVE_HWSERIAL1
  Serial1.begin(115200);
  Serial1.print('r');
  Serial1.flush();
  lastPrint = millis();
  #endif
}

void loop() {
  ard::EspConnection::instance().loop();
  ObjectStore::getInstance().loop();
#ifdef HAVE_HWSERIAL1
  // Make sure we're not working too fast for Arduino
  unsigned long now = millis();
  if ((now - lastPrint) >= PRINT_TIME) {
    lastPrint = now;
    Serial1.print('.');
    Serial1.flush();
  }
#endif
}