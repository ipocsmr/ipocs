
#include "ServerConnection.h"
#include "ObjectStore.h"
#include "Configuration.h"
#include <EEPROM.h>
#include <avr/wdt.h>

const long reconnectTime = 1000;

void reboot() {
  wdt_enable(WDTO_15MS); // turn on the WatchDog and don't stroke it.
  while (true); // do nothing and wait for the eventual...
}

ServerConnection::ServerConnection()
{
  this->lastReconnect = millis() - reconnectTime;
  Serial.print("Attempting to aquire IP from DHCP... ");
  byte mac[6];
  Configuration::getMAC(mac);
  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println(" failed, rebooting");
    Serial.flush();
    reboot();
  }
  // Print IP address
  Serial.println(Ethernet.localIP());
}

void ServerConnection::loadSaved()
{
  IPAddress ip = Configuration::getServer();
  this->setServer(ip);
}

void ServerConnection::setServer(IPAddress serverIP)
{
  this->serverIP = serverIP;
}

void ServerConnection::println(String data)
{
  if (!this->server.connected())
  {
    return;
  }
  this->server.println(data);
}

void ServerConnection::loop()
{
  if (!this->server.connected() && (millis() - this->lastReconnect > reconnectTime)) {
    Serial.print("(Re)connecting...");
    this->server.stop();
    this->server = EthernetClient();
    byte returnVal = this->server.connect(this->serverIP, 9999);
    if (returnVal == 1)
    {
      Serial.println(" done");
      this->inputString = "";
      this->println("unitid=" + String(Configuration::getUnitID(), 16));
    }
    else
    {
      Serial.print(String(' ') + String(returnVal));
      Serial.println(" failed");
    }
    this->lastReconnect = millis();
  } else if (this->server) {
    while (this->server.available()) {
      // get the new byte:
      char inChar = (char)this->server.read();
      // add it to the inputString:
      if (inChar == 'E') {
        if (this->inputString.length() == 0)
          continue;
        if (this->inputString[0] == 'P')
        {
          this->println("P");
        }
        else
        {
          if (this->inputString.length() == 2)
          {
            int objectNum = String(this->inputString[0]).toInt();
            int orderVal = String(this->inputString[1]).toInt();
            // Reset the input
            if ((objectNum > 0) && (objectNum <= 4)) {
              byte orderData[] = {orderVal};
              ObjectStore::getInstance().handleOrder(String("Points " + String(objectNum)), orderData, 1);
            }
          }
        }
        this->inputString = "";
      } else
        this->inputString += inChar;
    }
  }
}

