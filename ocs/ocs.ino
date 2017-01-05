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

#include "ServerConnection.h"
#include "BasicObject.h"
#include "ObjectStore.h"
#include "point_t1.h"

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte MJRIdator[] = {172, 16, 0, 105}; //Server IP address

const int Servoio[4] = {3, 5, 6, 9};

void setup() {
  MCUSR = 0;
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) ; // wait for serial port to connect. Needed for native USB port only
  Serial.println("USB Connected...");
  Serial.flush();

  ServerConnection::getInstance().setServer(MJRIdator);
  for (int index = 0; index < (sizeof(Servoio) / sizeof(int)); index++)
  {
    BasicObject* bo = new PointT1();
    byte configData[] = {Servoio[index]};
    bo->init(String("Points " + String(index + 1)), configData, 1);
    ObjectStore::getInstance().addObject(bo);
  }

  // give the Ethernet shield a second to initialize:
  delay(1000);
}

void loop() {
  ServerConnection::getInstance().loop();
  ObjectStore::getInstance().updateObjects();
}
//End loop
