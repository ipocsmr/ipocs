#include "Configuration.h"

#include <EEPROM.h>

IPAddress Configuration::getServer()
{
  byte b1 = EEPROM.read(2);
  byte b2 = EEPROM.read(3);
  byte b3 = EEPROM.read(4);
  byte b4 = EEPROM.read(5);
  return IPAddress(b1, b2, b3, b4);
}

void Configuration::setServer(IPAddress newIP)
{
  for (int i = 2; i < 6; i++)
  {
    EEPROM.write(i, newIP[i - 2]);
  }
}

unsigned int Configuration::getUnitID()
{
  unsigned int value = (EEPROM.read(0) << 8) + EEPROM.read(1);
  return value;
}

void Configuration::setUnitID(unsigned int unitID)
{
  EEPROM.write(0, unitID >> 8);
  EEPROM.write(1, unitID & 0xFF);
}

