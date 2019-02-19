#include "ServerConnection.h"
#include "Configuration.h"
#include "../IPOCS/Message.h"
#include "../IPOCS/Packets/ConnectionRequestPacket.h"
#include "../IPOCS/Packets/ApplicationDataPacket.h"
#include "../IPC/Message.h"
#include "http.h"
#include "ArduinoConnection.h"
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>

esp::ServerConnection::ServerConnection() {
    this->tcp = new WiFiClient();
}

esp::ServerConnection::~ServerConnection() {
    // this will never happen (as long as this class is a singleton) and generates warnings.
    //delete this->tcp;
}

esp::ServerConnection& esp::ServerConnection::instance() {
    static esp::ServerConnection _instance;
    return _instance;
}

void esp::ServerConnection::loop(bool isWiFiConnected) {
    if (!isWiFiConnected) {
        if (this->tcp->connected()) {
            this->tcp->stop();
        }
        return;
    }
    if (!this->tcp->connected()) {
        int responses = MDNS.queryService("ipocs", "tcp");
        esp::Http::instance().log("Number of mdns services: " + String(responses));
        if (responses != 0 && MDNS.IP(0) != IPAddress()) {
            IPAddress ip = MDNS.IP(0);
            uint16_t port = MDNS.port(0);
            esp::Http::instance().log("Connecting to: " + String(ip[3]) + "." + String(ip[2]) + "." + String(ip[1]) + "." + String(ip[0]) + ":" + String(port));
            byte returnVal = this->tcp->connect(ip, port);
            if (returnVal == 1) {
                char unitId[10];
                sprintf(unitId, "%d", Configuration::getUnitID());
                IPOCS::Message* msg = IPOCS::Message::create();
                msg->setObject(unitId);
                IPOCS::ConnectionRequestPacket* pkt = (IPOCS::ConnectionRequestPacket*)IPOCS::ConnectionRequestPacket::create();
                pkt->RM_PROTOCOL_VERSION = 0x0101;
                char sdVersion[10];
                sprintf(sdVersion, "%d", Configuration::getSiteDataCrc());
                pkt->RXID_SITE_DATA_VERSION = sdVersion;
                msg->setPacket(pkt);
                this->send(msg);
                delete msg;
            } else { }
        }
    } else {
        while (this->tcp->available()) {
            // Read the header
            byte RL_MESSAGE = 0;
            int numRead = this->tcp->read(&RL_MESSAGE, 1);
            if (numRead != 1) {
                // Close connection - error in reading.
                this->tcp->stop();
                return;
            }
            // Define the message
            byte message[RL_MESSAGE];
            message[0] = RL_MESSAGE;
            // Read the rest of the message
            numRead += this->tcp->read(message + 1, RL_MESSAGE - 1);
            if (numRead != RL_MESSAGE) {
                // Close connection - error in reading.
                this->tcp->stop();
                return;
            }
            // Now parse it.
            IPOCS::Message* msg = IPOCS::Message::create(message);
            IPC::Message* ipcMsg = IPC::Message::create();
            ipcMsg->RT_TYPE = IPC::IPOCS;
            ipcMsg->setPayload(message, msg->RL_MESSAGE);

            char unitId[10];
            sprintf(unitId, "%d", Configuration::getUnitID());
            if (strcmp(msg->getObject(), unitId) == 0) {
                if (msg->packet->RNID_PACKET == 5) {
                    IPOCS::ApplicationDataPacket* dataPkt = (IPOCS::ApplicationDataPacket* const)msg->packet;
                    uint8_t dataLength = dataPkt->RL_PACKET - 5;
                    Configuration::setSD(dataPkt->data, dataLength);
                    esp::Http::instance().handleRestart(true);
                }
            } else {
                ArduinoConnection::instance().send(ipcMsg);
            }
            delete msg;
            delete ipcMsg;
        }
    }
}

void esp::ServerConnection::send(IPOCS::Message* msg)
{
  if (!this->tcp->connected()) {
    esp::Http::instance().log("Not connected, throwing message");
    return;
  }
  uint8_t buffer[255];
  uint8_t len = msg->serialize(buffer);
  this->tcp->write(buffer, len);
}