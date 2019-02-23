#include "Configuration.h"

#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <uCRC16Lib.h>

uint16_t Configuration::getUnitID()
{
  uint16_t value = (EEPROM.read(0) << 8) + EEPROM.read(1);
  return value;
}

void Configuration::setUnitID(uint16_t unitID)
{
  EEPROM.write(0, unitID >> 8);
  EEPROM.write(1, unitID & 0xFF);
  EEPROM.commit();
}

size_t Configuration::getSSID(char ssid[]) {
  for (uint16_t i = 0; i < 33; i++) {
    ssid[i] = EEPROM.read(i + 2);
  }
  ssid[32] = 0; // Force null termination for uninitialized cards
  return strlen(ssid);
}

void Configuration::setSSID(const char ssid[])
{
  for (uint16_t i = 0; i < strlen(ssid); i++) {
    EEPROM.write(i + 2, ssid[i]);
  }
  EEPROM.write(strlen(ssid) + 2 , 0);
  EEPROM.commit();
}

size_t Configuration::getPassword(char pwd[])
{
  for (uint16_t i = 0; i < 61; i++) {
    pwd[i] = EEPROM.read(i + 36);
  }
  pwd[61] = 0; // Force null termination for uninitialized cards
  return strlen(pwd);
}

void Configuration::setPassword(const char pwd[])
{
  for (uint16_t i = 0; i < strlen(pwd); i++) {
    EEPROM.write(i + 36, pwd[i]);
  }
  EEPROM.write(strlen(pwd) + 36, 0);
  EEPROM.commit();
}

uint8_t Configuration::getSD(uint8_t data[], int dataLength)
{
  uint8_t storedLength = EEPROM.read(100);
  // For unitialized memory, the value is 0xFF. If that occurs - return 0 and nothing will be loaded.
  if (storedLength == 0xFF)
    storedLength = 0x00;
  for (int i = 0; i < storedLength; i++)
  {
    data[i] = EEPROM.read(101 + i);
  }

  return storedLength;
}

void Configuration::setSD(uint8_t data[], int dataLength)
{
  EEPROM.write(100, dataLength);
  for (int i = 0; i < dataLength; i++)
  {
    EEPROM.write(101 + i, data[i]);
  }
  uint16_t crc = uCRC16Lib::calculate(data, dataLength);
  EEPROM.write(98, crc >> 8);
  EEPROM.write(99, crc & 0xFF);
  EEPROM.commit();
}

uint16_t Configuration::getSiteDataCrc()
{
  return (EEPROM.read(98) << 8) + EEPROM.read(99);
}

bool Configuration::verifyCrc()
{
  uint16_t storedCrc = Configuration::getSiteDataCrc();

  uint8_t data[100];
  uint8_t sdLength = Configuration::getSD(data, 0x100);
  uint16_t calculatedCrc = uCRC16Lib::calculate(data, sdLength);
  return (sdLength != 0 && calculatedCrc == storedCrc);
}

__attribute__((constructor))
static void initialize_packet_applicationdata() {
  EEPROM.begin(1024);
}
