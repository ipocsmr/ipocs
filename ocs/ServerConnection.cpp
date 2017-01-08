
#include "ServerConnection.h"
#include "ObjectStore.h"
#include "Configuration.h"
#include <EEPROM.h>
#include <avr/wdt.h>
#include "IPOCS.h"

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

void ServerConnection::send(IPOCS::Message* msg)
{
  if (!this->server.connected())
  {
    return;
  }
  uint8_t buffer[255];
  uint8_t len = msg->serialize(buffer);
  this->server.write(buffer, len);
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
      IPOCS::Message* msg = IPOCS::Message::create();
      msg->RXID_OBJECT = String((char)Configuration::getUnitID());

      IPOCS::ConnectionRequestPacket* pkt = (IPOCS::ConnectionRequestPacket*)IPOCS::ConnectionRequestPacket::create();
      pkt->RM_PROTOCOL_VERSION = 0x0101;
      pkt->RXID_SITE_DATA_VERSION = "1.0";
      msg->addPacket(pkt);

      this->send(msg);
      msg->destroy();
      delete msg;
    }
    else
    {
      Serial.print(String(' ') + String(returnVal));
      Serial.println(" failed");
    }
    this->lastReconnect = millis();
  } else if (this->server) {
    while (this->server.available()) {
      // Read the header
      byte RL_MESSAGE = 0;
      int numRead = this->server.read(&RL_MESSAGE, 1);
      if (numRead != 1)
      {
        // Close connection - error in reading.
        this->server.stop();
        return;
      }
      // Define the message
      byte message[RL_MESSAGE];
      message[0] = RL_MESSAGE;
      // Read the rest of the message
      numRead += this->server.read(message + 1, RL_MESSAGE - 1);
      if (numRead != RL_MESSAGE)
      {
        // Close connection - error in reading.
        this->server.stop();
        return;
      }
      // Now parse it.
      IPOCS::Message* msg = IPOCS::Message::create(message);
      ObjectStore::getInstance().handleOrder(msg);
      msg->destroy();
      delete msg;
      /*
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
        */
    }
  }
}

