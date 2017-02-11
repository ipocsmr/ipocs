/*
  Connection to JMRI server

  This sketch connects to the JMRI server
  using an Arduino Wiznet Ethernet shield.

  Circuit:
   Ethernet shield attached to pins 10, 11, 12, 13

  created 8 Dec 200916
  by P Elestedt

*/

#include <Arduino.h>
#include "CommandInterface.h"
#include "ServerConnection.h"
#include "ObjectStore.h"

void setup() {
  CommandInterface::getInstance().setup();
  ServerConnection::getInstance().setup();
  ObjectStore::getInstance().setup();
}

void loop() {
  CommandInterface::getInstance().loop();
  ServerConnection::getInstance().loop();
  ObjectStore::getInstance().loop();
}
