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
#ifdef ESP8266
  EEPROM.commit();
#endif
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
#ifdef ESP8266
  EEPROM.commit();
#endif
}

String Configuration::getSSID()
{
  byte ssid[6];
  Configuration::getMAC(ssid);
  String s_ssid;
  for (int i = 0; i < 6; i++)
    s_ssid += String((char)ssid[i]);
  return s_ssid;
}

void Configuration::setSSID(String ssid)
{
  byte bssid[7];
  ssid.getBytes(bssid, 7);
  Configuration::setMAC(bssid);
}

String Configuration::getPassword()
{
  String pwd;
  for (int i = 0; i < 8; i++)
    pwd += String((char)EEPROM.read(24 + i));
  return pwd;
}

void Configuration::setPassword(String pwd)
{
  byte bpwd[9];
  pwd.getBytes(bpwd, 9);
  for (int i = 0; i < 8; i++)
  {
    EEPROM.write(24 + i, bpwd[i]);
  }
#ifdef ESP8266
  EEPROM.commit();
#endif
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
#ifdef ESP8266
  EEPROM.commit();
#endif
}

byte Configuration::getSD(byte data[], int dataLength)
{
  byte storedLength = EEPROM.read(32);
  // For unitialized memory, the value is 0xFF. If that occurs - return 0 and nothing will be loaded.
  if (storedLength == 0xFF)
    storedLength = 0x00;
  for (int i = 0; i < storedLength; i++)
  {
    data[i] = EEPROM.read(33 + i);
  }
  return storedLength;
}

void Configuration::setSD(byte data[], int dataLength)
{
  EEPROM.write(32, dataLength);
  for (int i = 0; i < dataLength; i++)
  {
    EEPROM.write(33 + i, data[i]);
  }
#ifdef ESP8266
  EEPROM.commit();
#endif
}

#ifdef ESP8266
__attribute__((constructor))
static void initialize_config() {
  EEPROM.begin(512);
}
#endif
