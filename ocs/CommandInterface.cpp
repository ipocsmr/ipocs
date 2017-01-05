
#include "CommandInterface.h"
#include "Configuration.h"
#include <SPI.h>
#include <Ethernet.h>
#include <stdlib.h>

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
    /*
    Serial.println("");
    Serial.println("Available commands:");
    Serial.println("\tget <property>");
    Serial.println("\tset <property> <value>");
    Serial.println("\texit");
    Serial.println("");
    Serial.println("Available properties");
    Serial.println("\tunitid - Identity of this card for the GMJS OCS protocol. Range: 1-65535");
    Serial.println("\tserver - IPv4 address to server. Format: stardard IPv4 notation");
    Serial.println("\tmac - MAC Address of the card. Format: XX XX XX XX XX XX");
    Serial.println("\tdata - Site configuration data. Format: hex list separated by space.\n\tFirst byte is the length of the data.");
    Serial.println("");
    */
  }
  else if (cmd == "get")
  {
    if (command == "unitid")
    {
      unsigned int value = Configuration::getUnitID();
      Serial.println("unitid = " + String(value));
    }
    else if (command == "server")
    {
      IPAddress ip = Configuration::getServer();
      Serial.println("server = " + String(ip[0]) + "."
                                 + String(ip[1]) + "."
                                 + String(ip[2]) + "."
                                 + String(ip[3]));
    }
    else if (command == "mac")
    {
      byte mac[6];
      Configuration::getMAC(mac);
      Serial.println("mac = " + String(mac[0], 16) + ":"
                              + String(mac[1], 16) + ":"
                              + String(mac[2], 16) + ":"
                              + String(mac[3], 16) + ":"
                              + String(mac[4], 16) + ":"
                              + String(mac[5], 16));
    }
    else if (command == "data")
    {
      byte sd[200];
      byte sdLength = Configuration::getSD(sd, 200);
      Serial.print("data = ");
      for (int i = 0; i < sdLength; i++)
      {
        Serial.print(String(sd[i], 16) + ' ');
      }
      Serial.println();
    }
    else
    {
      Serial.println("Unknown property. Try again, or type help.");
      Serial.println();
    }
  }
  else if (cmd == "set")
  {
    index = command.indexOf(' ');
    String subCmd = command.substring(0, index);
    command.remove(0, command.indexOf(' ') + 1);
    if (subCmd == "unitid")
    {
      Configuration::setUnitID(command.toInt());
      this->handleCommand("get unitid");
    }
    else if (subCmd == "server")
    {
      IPAddress ip;
      ip.fromString(command);
      Configuration::setServer(ip);
      this->handleCommand("get server");
    }
    else if (subCmd == "mac")
    {
      byte mac[6];
      char* pos = command.c_str();
      for (int i = 0; i < 6; i++)
      {
        mac[i] = strtol(pos, &pos, 16);  
      }
      Configuration::setMAC(mac);
      this->handleCommand("get mac");
    }
    else if (subCmd == "data")
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
      this->handleCommand("get data");
    }
    else
    {
      Serial.println("Unknown property. Try again, or type help.");
      Serial.println();
    }
  }
  else if (cmd == "exit")
  {
    return false;
  }
  else
  {
    Serial.println("Unknown command. Try again, or type help.");
    Serial.println();
  }
  return true;
}


