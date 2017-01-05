
#include "CommandInterface.h"
#include <SPI.h>
#include <EEPROM.h>

const long waitAtStartTime = 4000;

void CommandInterface::setup()
{
  Serial.begin(9600);
  Serial.println("To enter setup mode - press 's' within 4 seconds");
  Serial.flush();
  long startedAt = millis();
  while (millis() - startedAt <= waitAtStartTime)
  {
    if (!Serial)
    {
      Serial.begin(9600);
      continue;
    }
    if (Serial.available()) {
      char inChar = Serial.read();
      if (inChar == 's' || inChar == 'S') {
        while (Serial.available()) {
          char inChar = Serial.read();
        }
        this->setupMode();
        break;
        Serial.println("Exited setup, resuming start");
        Serial.flush();
      }
    }
  }
}

void CommandInterface::loop()
{

}

void CommandInterface::setupMode()
{
  bool inSetupMode = true;
  bool hasPrinted = false;
  String inString = "";
  Serial.println("Setup mode. Enter command, or help for a list of commands");
  while (inSetupMode)
  {
    if (!hasPrinted)
    {
      Serial.print("$> ");
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
        Serial.println("");
        inSetupMode = this->handleCommand(inString);
        inString = "";
      }
    }
  }
}

bool CommandInterface::handleCommand(String command)
{
  int index = command.indexOf(' ');
  String cmd = command.substring(0, index);
  command.remove(0, index + 1);
  if (cmd == "help")
  {
    Serial.println("");
    Serial.println("Available commands:");
    Serial.println("\tget <property>");
    Serial.println("\tset <property> <value>");
    Serial.println("\texit");
    Serial.println("");
    Serial.println("Available properties");
    Serial.println("\tunitid - Identity of this card for the GMJS OCS protocol. Range: 1-65535");
    Serial.println("");
  }
  else if (cmd == "get")
  {
    if (command == "unitid")
    {
      unsigned int value = (EEPROM.read(0) << 8) + EEPROM.read(1);
      Serial.println("unitid = " + String(value));
    }
    else
    {
      Serial.println("Unknown property. Try again, or type help.");
      Serial.println("");
    }
  }
  else if (cmd == "set")
  {
    index = command.indexOf(' ');
    String subCmd = command.substring(0, index);
    command.remove(0, command.indexOf(' ') + 1);
    if (subCmd == "unitid")
    {
      unsigned int value = command.toInt();
      EEPROM.write(0, value >> 8);
      EEPROM.write(1, value & 0xFF);
      Serial.println("unitid = " + String(value));
    }
    else
    {
      Serial.println("Unknown property. Try again, or type help.");
      Serial.println("");
    }
  }
  else if (cmd == "exit")
  {
    return false;
  }
  else
  {
    Serial.println("Unknown command. Try again, or type help.");
    Serial.println("");
  }
  return true;
}


