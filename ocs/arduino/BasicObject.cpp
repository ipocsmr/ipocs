
#include "BasicObject.h"

void BasicObject::init(String& objectName, const uint8_t* configData, int configDataLen)
{
  this->objectName = new char[objectName.length() + 1];
  objectName.toCharArray(this->objectName, objectName.length() + 1);
  this->objectInit(configData, configDataLen);
}

bool BasicObject::hasName(const char* const objectName) {
  return strcmp(this->objectName, objectName) == 0;
}

