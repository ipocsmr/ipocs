
#include "CommandInterface.h"
#include "Configuration.h"
#include <SPI.h>
#include <Ethernet.h>
#include <stdlib.h>

const long waitAtStartTime = 4000;
const char TO_ENTER_SETUP[] = "To enter setup mode - press 's' within 4 seconds";
const char PROMPT[] = "$> ";
const char EQUALS[] = " = ";
const char CMD_GET[] = "get";
const char CMD_SET[] = "set";
const char CMD_RESET[] = "reset";
const char CMD_EXIT[] = "exit";
const char CMD_CMD_UNITID[] = "unitid";
const char CMD_CMD_MAC[] = "mac";
const char CMD_CMD_PWD[] = "pwd";
const char CMD_CMD_SSID[] = "ssid";
const char CMD_CMD_DATA[] = "data";

void CommandInterface::setup()
{
  Serial.begin(115200);
  Serial.println(TO_ENTER_SETUP);
  Serial.flush();
  long startedAt = millis();
  unsigned int unitId = Configuration::getUnitID();
  while (millis() - startedAt <= waitAtStartTime || unitId == 0xFFFF)
  {
    if (!Serial)
    {
      Serial.begin(115200);
      continue;
    }
    if (Serial.available()) {
      char inChar = Serial.read();
      if (inChar == 's' || inChar == 'S' || unitId == 0xFFFF) {
        while (Serial.available()) {
          inChar = Serial.read();
        }
        this->setupMode();
        break;
      }
    }
  }
  if (Configuration::getUnitID() == 0xFFFF)
  {
    // 0xFF in a byte indicates uninitialized memory.
    // Force setup mode...
    this->setupMode();
  }
}

void CommandInterface::loop()
{

}

void CommandInterface::setupMode()
{
  bool inSetupMode = true;
  bool hasPrinted = false;
  String inString;
  while (inSetupMode)
  {
    if (!hasPrinted)
    {
      Serial.print(PROMPT);
      hasPrinted = true;
    }
    if (Serial.available())
    {
      const char inChar = Serial.read();
      if (inChar != '\n') {
        if (inChar != '\r') {
          inString += inChar;
          Serial.print(inChar);
        }
      } else {
        if (inString.length() == 0)
          continue;
        hasPrinted = false;
        Serial.println();
        inSetupMode = this->handleCommand(inString);
        inString = "";
      }
    }
  }
}

void CommandInterface::printArray(String pre, byte arr[], byte arrLen)
{
  this->printArray(pre, arr, arrLen, ':', 16);
}

void CommandInterface::printArray(String pre, byte arr[], byte arrLen, char divider, int base = 10)
{
  Serial.print(pre + String(EQUALS));
  for (int i = 0; i < arrLen; i++)
  {
    Serial.print(String(arr[i], base) + String((i != (arrLen - 1)) ? divider : ' '));
  }
  Serial.println();
}

bool CommandInterface::handleCommand(String command)
{
  int index = command.indexOf(' ');
  String cmd = command.substring(0, index);
  command.remove(0, index + 1);
  if (cmd == CMD_RESET)
  {
    ipocsResetFunc f = 0;
    f();
  }
  if (cmd == CMD_GET)
  {
    if (command == CMD_CMD_UNITID)
    {
      unsigned int value = Configuration::getUnitID();
      Serial.println(String(CMD_CMD_UNITID) + String(EQUALS) + String(value));
    }
    else if (command == CMD_CMD_MAC)
    {
      byte mac[6];
      Configuration::getMAC(mac);
      this->printArray(CMD_CMD_MAC, mac, 6);
    }
    else if (command == CMD_CMD_PWD)
    {
      Serial.println(String(CMD_CMD_PWD) + String(EQUALS) + Configuration::getPassword());
    }
    else if (command == CMD_CMD_SSID)
    {
      Serial.println(String(CMD_CMD_SSID) + String(EQUALS) + Configuration::getSSID());
    }
    else if (command == CMD_CMD_DATA)
    {
      byte sd[200];
      byte sdLength = Configuration::getSD(sd, 200);
      this->printArray(CMD_CMD_DATA, sd, sdLength, ' ', 16);
    }
    else
    {
      Serial.println('?');
    }
  }
  else if (cmd == CMD_SET)
  {
    index = command.indexOf(' ');
    String subCmd = command.substring(0, index);
    command.remove(0, command.indexOf(' ') + 1);
    if (subCmd == CMD_CMD_UNITID)
    {
      Configuration::setUnitID(command.toInt());
    }
    else if (subCmd == CMD_CMD_MAC)
    {
      byte mac[6];
      char* pos = (char*)command.c_str();
      for (int i = 0; i < 6; i++)
      {
        mac[i] = strtol(pos, &pos, 16);
      }
      Configuration::setMAC(mac);
    }
    else if (subCmd == CMD_CMD_SSID)
    {
      Configuration::setSSID(command);
    }
    else if (subCmd == CMD_CMD_PWD)
    {
      Configuration::setPassword(command);
    }
    else if (subCmd == CMD_CMD_DATA)
    {
      byte sd[200];
      int sdLength = 0;
      while (command.length() != 0)
      {
        int spPos = command.indexOf(' ');
        if (spPos == -1)
          spPos = 2;
        String byteStr = command.substring(0, spPos);
        command.remove(0, spPos + 1);
        sd[sdLength++] = strtol(byteStr.c_str(), NULL, 16);
      }
      Configuration::setSD(sd, sdLength);
    }
    else
    {
      Serial.println('?');
    }
  }
  else if (cmd == CMD_EXIT)
  {
    return false;
  }
  else
  {
    Serial.println('?');
  }
  return true;
}
