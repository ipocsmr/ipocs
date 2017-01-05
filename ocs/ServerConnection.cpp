
#include "ServerConnection.h"
#include "ObjectStore.h"
#include <avr/wdt.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
const long reconnectTime = 1000;

void reboot() {
  wdt_enable(WDTO_15MS); // turn on the WatchDog and don't stroke it.
  while (true); // do nothing and wait for the eventual...
}

ServerConnection::ServerConnection()
{
  this->lastReconnect = millis() - reconnectTime;
  Serial.print("Attempting to aquire IP from DHCP...");
  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("");
    Serial.println("Failed to configure Ethernet using DHCP, rebooting");
    Serial.flush();
    reboot();
  }
  //Print IP address
  Serial.print(" ");
  Serial.println(Ethernet.localIP());
}

void ServerConnection::setServer(byte server[4])
{
  memcpy(this->serverIP, server, 4);
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
    this->server.setClientTimeout(100);
    byte returnVal = this->server.connect(this->serverIP, 9999);
    if (returnVal == 1)
    {
      Serial.println(" done");
      this->inputString = "";
    }
    else
    {
      Serial.println(" failed");
    }
    this->lastReconnect = millis();
  } else if (this->server) {
    while (this->server.available()) {
      // get the new byte:
      char inChar = (char)this->server.read();
      // add it to the inputString:
      if (inChar == 'E') {
        long order = strtoul(this->inputString.c_str(), NULL, 16);
        // Reset the input
        this->inputString = "";
        if (order != 0) {
          int orderVal = order & 0x0F;
          int objectNum = order / 10;
          if ((objectNum > 0) && (objectNum <= 4)) {
            byte orderData[] = {orderVal};
            ObjectStore::getInstance().handleOrder(String("Points " + String(objectNum)), orderData, 1);
          }
          order = 0;
        }
      } else
        this->inputString += inChar;
    }
  }
}

