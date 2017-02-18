
#include "ServerConnection.h"
#include "ObjectStore.h"
#include "Configuration.h"
#include <EEPROM.h>
#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif
#include <Ethernet.h>
#include "IPOCS.h"

const long reconnectTime = 1000;

void WiFiConnect()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    String ssid = Configuration::getSSID();
    String pwd = Configuration::getPassword();
    WiFi.begin((char*)ssid.c_str(), pwd.c_str());
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
    }
  }
}

ServerConnection::ServerConnection()
{
  this->lastReconnect = millis() - reconnectTime;

  if (WiFi.status() == WL_NO_SHIELD)
  {
    // No WiFi shield. Assume Ethernet
    byte mac[6];
    Configuration::getMAC(mac);
    // start the Ethernet connection:
    if (Ethernet.begin(mac) == 0) {
      Serial.println(" failed, rebooting");
      Serial.flush();
      ipocsResetFunc f = 0;
      f();
    }
    this->server = new EthernetClient();
  }
  else
  {
    this->server = new WiFiClient();
    WiFiConnect();
  }
  udp.begin(10000);
}

void ServerConnection::setup()
{
}

void ServerConnection::send(IPOCS::Message* msg)
{
  if (!this->server->connected())
  {
    return;
  }
  uint8_t buffer[255];
  uint8_t len = msg->serialize(buffer);
  this->server->write(buffer, len);
}

void ServerConnection::loop()
{
  if (!this->server->connected() && (millis() - this->lastReconnect > reconnectTime)) {
    this->lastReconnect = millis();
    if (WiFi.status() != WL_NO_SHIELD)
    {
      WiFiConnect();
    }
    Serial.print("(Re)connecting...");
    this->server->stop();

    int packetSize = this->udp.parsePacket();
    if (packetSize == 0)
    {
      this->udp.beginPacket("255.255.255.255", 10000);
      this->udp.write("get server");
      this->udp.endPacket();
      Serial.println(" sent UDP.");
      Serial.flush();
    }
    else
    {
      char packetBuffer[40];
      this->udp.read(packetBuffer, 40);

      IPAddress ip(packetBuffer[0], packetBuffer[1], packetBuffer[2], packetBuffer[3]);
      int port = (packetBuffer[4] << 8) + packetBuffer[5];
      byte returnVal = this->server->connect(ip, port);
      if (returnVal == 1)
      {
        Serial.println(" done");
        IPOCS::Message* msg = IPOCS::Message::create();
        msg->RXID_OBJECT = String((char)Configuration::getUnitID());
        IPOCS::ConnectionRequestPacket* pkt = (IPOCS::ConnectionRequestPacket*)IPOCS::ConnectionRequestPacket::create();
        pkt->RM_PROTOCOL_VERSION = 0x0101;
        pkt->RXID_SITE_DATA_VERSION = "1.0";
        msg->setPacket(pkt);

        this->send(msg);
        delete msg;
      }
      else
      {
        Serial.print(String(' ') + String(returnVal));
        Serial.println(" failed");
      }
    }
  } else if (*this->server) {
    while (this->server->available()) {
      // Read the header
      byte RL_MESSAGE = 0;
      int numRead = this->server->read(&RL_MESSAGE, 1);
      if (numRead != 1)
      {
        // Close connection - error in reading.
        this->server->stop();
        return;
      }
      // Define the message
      byte message[RL_MESSAGE];
      message[0] = RL_MESSAGE;
      // Read the rest of the message
      numRead += this->server->read(message + 1, RL_MESSAGE - 1);
      if (numRead != RL_MESSAGE)
      {
        // Close connection - error in reading.
        this->server->stop();
        return;
      }
      // Now parse it.
      IPOCS::Message* msg = IPOCS::Message::create(message);
      ObjectStore::getInstance().handleOrder(msg);
      delete msg;
    }
  }
}
