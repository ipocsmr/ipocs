#include "BasicObject.h"
#include <string.h>

void BasicObject::init(const char objectName[], const uint8_t* configData, int configDataLen)
{
  this->objectName = new char[strlen(objectName) + 1];
  strcpy(this->objectName, objectName);
  this->objectInit(configData, configDataLen);
}

bool BasicObject::hasName(const char* const objectName) {
  return strcmp(this->objectName, objectName) == 0;
}

