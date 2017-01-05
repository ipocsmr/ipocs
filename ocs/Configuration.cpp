#include "Configuration.h"

#include <EEPROM.h>

IPAddress Configuration::getServer()
{
  byte b1 = EEPROM.read(8);
  byte b2 = EEPROM.read(9);
  byte b3 = EEPROM.read(10);
  byte b4 = EEPROM.read(11);
  return IPAddress(b1, b2, b3, b4);
}

void Configuration::setServer(IPAddress newIP)
{
  for (int i = 8; i < 12; i++)
  {
    EEPROM.write(i, newIP[i - 8]);
  }
}

void Configuration::getMAC(byte MAC[6])
{
  MAC[0] = EEPROM.read(2);
  MAC[1] = EEPROM.read(3);
  MAC[2] = EEPROM.read(4);
  MAC[3] = EEPROM.read(5);
  MAC[4] = EEPROM.read(6);
  MAC[5] = EEPROM.read(7);
}

void Configuration::setMAC(byte newMAC[6])
{
  for (int i = 2; i < 8; i++)
  {
    EEPROM.write(i, newMAC[i - 2]);
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

byte Configuration::getSD(byte data[], int dataLength)
{
  byte storedLength = EEPROM.read(24);
  for(int i = 0; i < storedLength; i++)
  {
    data[i] = EEPROM.read(25 + i);
  }
  return storedLength;
}

void Configuration::setSD(byte data[], int dataLength)
{
  EEPROM.write(24, dataLength);
  for (int i = 0; i < dataLength; i++)
  {
    EEPROM.write(25 + i, data[i]);
  }
}

