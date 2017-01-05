#include <SPI.h>

#include "ObjectStore.h"
#include "point_t1.h"
#include "ServerConnection.h"

const int normal = 20;
const int thrown = 150;
const int pointStepTime = 30;

PointT1::PointT1()
{
}

void PointT1::objectInit(byte configData[], int configDataLen)
{
  this->lastRun = millis();
  this->setPos = normal;
  this->curPos = normal + 1;
  if (configDataLen == 1)
  {
    Serial.println("Attaching point to pin " + String(configData[0]));
    this->object.attach(configData[0]);
  }
}

byte PointT1::getStatus()
{

}

void PointT1::handleOrder(byte orderVec[], int orderVecLen)
{
  if (orderVecLen != 1)
  {
    return;
  }
  switch (orderVec[0])
  {
    case 0: this->setPos = normal; break;
    case 1: this->setPos = thrown; break;
    default:
      // Handle invalid order....
      break;
  }
}

void PointT1::update()
{
  if (millis() - this->lastRun > pointStepTime) {
    this->lastRun = millis();
    // For each object, handle it.
    if (this->setPos == this->curPos)
      return;
    // Else determine which direction to move.
    int direction = this->setPos > this->curPos ? +1 : -1;
    // And then move
    this->curPos += direction;
    this->object.write(this->curPos);
    // And possibly notify on reaching end position (obsolete if we have an input to look at)
    if (this->curPos == this->setPos)
    {
      String dir = this->setPos == thrown ? String("Thrown") : String("Normal");
      Serial.println(this->objectName + String(" in " + dir + " position"));
      ServerConnection::getInstance().println(this->objectName + String(" in " + dir + " position"));
    }
  }
}

static BasicObject* createPointT1()
{
  return new PointT1();
}

__attribute__((constructor))
static void initialize_point_t1() {
  ObjectStore::getInstance().registerType(4, &createPointT1);
}
