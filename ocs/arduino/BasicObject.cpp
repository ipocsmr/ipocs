
#include "BasicObject.h"

void BasicObject::init(String objectName, const uint8_t* configData, int configDataLen)
{
  this->objectName = objectName;
  this->objectInit(configData, configDataLen);
}

bool BasicObject::hasName(String objectName) {
  return this->objectName == objectName;
}

