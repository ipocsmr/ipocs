/*
  Connection to JMRI server

  This sketch connects to the JMRI server
  using an Arduino Wiznet Ethernet shield.

  Circuit:
   Ethernet shield attached to pins 10, 11, 12, 13

  created 8 Dec 200916
  by P Elestedt

*/

#include <SPI.h>
#include <Ethernet.h>
#include <Servo.h>
#include <stdlib.h>

#include "CommandInterface.h"
#include "ServerConnection.h"
#include "BasicObject.h"
#include "ObjectStore.h"
#include "point_t1.h"

const int Servoio[4] = {3, 5, 6, 9};

void setup() {
  MCUSR = 0;
  CommandInterface::getInstance().setup();
  for (int index = 0; index < (sizeof(Servoio) / sizeof(int)); index++)
  {
    BasicObject* bo = new PointT1();
    byte configData[] = {Servoio[index]};
    bo->init(String("Points " + String(index + 1)), configData, 1);
    ObjectStore::getInstance().addObject(bo);
  }
  ServerConnection::getInstance().loadSaved();
}

void loop() {
  CommandInterface::getInstance().loop();  
  ServerConnection::getInstance().loop();
  ObjectStore::getInstance().updateObjects();
}
//End loop
