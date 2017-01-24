
#include "PointsMotorStore.h"
#include <string.h>

PointsMotorStore::PointsMotorStore()
{
  memset(this->functions, '\0', 2);
}

void PointsMotorStore::registerType(int typeId, initPointsMotorFunction fun)
{
  this->functions[typeId - 1] = fun;
}

void PointsMotorStore::getFunction(int typeId, initPointsMotorFunction* fun)
{
  (*fun) = this->functions[typeId - 1];
}

