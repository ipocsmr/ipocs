
#include "ServerConnection.h"
#include "ObjectStore.h"
#include "Configuration.h"
#include "IPOCS/Message.h"
#include "IPOCS/Packets/ConnectionRequestPacket.h"
#include <ESP8266.h>

const long reconnectTime = 1000;

ServerConnection::ServerConnection() {
  this->lastReconnect = millis() - reconnectTime;
  this->serverIP = NULL;
  String ssid = Configuration::getSSID();
  String pwd = Configuration::getPassword();
#ifdef HAVE_HWSERIAL3
  Serial.println("Booting card...");
  Serial.flush();
#endif
#if defined(HAVE_HWSERIAL3)
  //HardwareSerial wifiSerial = Serial3;
  this->wifi = new ESP8266(Serial3);
#else
  //HardwareSerial wifiSerial = Serial;
  this->wifi = new ESP8266(Serial);
#endif
  if (!this->wifi->restart()) {
#ifdef HAVE_HWSERIAL3
    Serial.println("Restart failed");
    Serial.flush();
#endif
    ipocsResetFunc f = 0;
    f();
  }
  if (!this->wifi->kick())
  {
#ifdef HAVE_HWSERIAL3
    Serial.println("Kick failed");
    Serial.flush();
#endif
    ipocsResetFunc f = 0;
    f();
  }
  this->wifi->enableMUX();
  this->wifi->joinAP(ssid, pwd);
  this->wifi->registerUDP(1, "255.255.255.255", 10000);
}

void ServerConnection::setup() { }

void ServerConnection::send(IPOCS::Message* msg) {
  if (this->serverIP == NULL || -1 == this->wifi->getIPStatus().indexOf(*(this->serverIP))) {
    return;
  }
  uint8_t buffer[255];
  uint8_t len = msg->serialize(buffer);
  this->wifi->send(2, buffer, len);
}

void ServerConnection::loop() {
#if CARD==ROBOTDYN_WIFI
  if (this->serverIP == NULL) {
    uint8_t recvBuffer[40];
    uint32_t bytesRecv = this->wifi->recv(1, recvBuffer, 4);
    if (bytesRecv == 0) {
      String getData = "get server";
      if (!this->wifi->send(1, (const uint8_t*)getData.c_str(), getData.length()))
      {
        Serial.println("Sending failed");
        // TODO: Handle not being able to send UDP
        return;
      }
      return;
    } 
    if (bytesRecv == 4) {
#ifdef HAVE_HWSERIAL3
      Serial.print("Connecting...");
#endif
      char ipAddress[24];
      sprintf(ipAddress, "%d.%d.%d.%d", recvBuffer[0], recvBuffer[1], recvBuffer[2], recvBuffer[3]);
      String* newIp = new String(ipAddress);
      if (this->wifi->createTCP(2, *(newIp), 10000U)) {
        this->serverIP = newIp;
#ifdef HAVE_HWSERIAL3
        Serial.println(" done");
        Serial.flush();
#endif
        IPOCS::Message* msg = IPOCS::Message::create();
        msg->RXID_OBJECT = String((char)Configuration::getUnitID());
        IPOCS::ConnectionRequestPacket* pkt = (IPOCS::ConnectionRequestPacket*)IPOCS::ConnectionRequestPacket::create();
        pkt->RM_PROTOCOL_VERSION = 0x0101;
        char sdVersion[10];
        sprintf(sdVersion, "%04X", Configuration::getSiteDataCrc());
        pkt->RXID_SITE_DATA_VERSION = String(sdVersion);
        msg->setPacket(pkt);
        this->send(msg);
        delete msg;
      } else {
#ifdef HAVE_HWSERIAL3
        Serial.println(" failed");
        Serial.flush();
#endif
      }
    }
    return;
  }
    
  // Check if getIPStatus contains the connection, if it doesn't - connect again.
  if (-1 == this->wifi->getIPStatus().indexOf(*(this->serverIP))) {
#ifdef HAVE_HWSERIAL3
    Serial.println("No connection");
#endif
    delete this->serverIP;
    this->serverIP = NULL;
    this->wifi->releaseTCP(2);
    return;
  }

  // There was a byte. Now read more
  // Read the header
  uint8_t muxId = 0;
  uint8_t message[255];
  uint32_t numRead = this->wifi->recv(&muxId, message, 255, 200U);
  if (numRead == 0) {
    return;
  }
  if (muxId != 2) {
    return;
  }
  // Define the message
  if (numRead != message[0]) {
    // Close connection - error in reading.
    delete this->serverIP;
    this->serverIP = NULL;
    this->wifi->releaseTCP(2);
    this->wifi->registerUDP(1, "255.255.255.255", 10000);
    return;
  }
  // Now parse it.
  IPOCS::Message* msg = IPOCS::Message::create(message);
  ObjectStore::getInstance().handleOrder(msg);
  delete msg;
#endif // CARD == ROBOTDYN_WIFI
}
