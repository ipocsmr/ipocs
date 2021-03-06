#include "../communications/ServerConnection.h"
#include "../communications/Configuration.h"
#include "../IPOCS/Message.h"
#include "../IPOCS/Packets/ConnectionRequestPacket.h"
#include "../IPOCS/Packets/ApplicationDataPacket.h"
#include "../IPC/Message.h"
#include "../LedControl.h"
#include "http.h"
#include "../communications/RestartManager.h"
#include "../communications/ArduinoConnection.h"
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>

MDNSResponder::hMDNSServiceQuery hMDNSServiceQuery;

void MDNSServiceQueryCallback(MDNSResponder::MDNSServiceInfo serviceInfo, MDNSResponder::AnswerType answerType, bool p_bSetContent) {
    switch (answerType) {
        case MDNSResponder::AnswerType::IP4Address: {
            IPAddress localIP = WiFi.localIP();
            for (IPAddress ip : serviceInfo.IP4Adresses()) {
                if (localIP[0] == ip[0] && localIP[1] == ip[1] && localIP[2] == ip[2]) {
                    esp::ServerConnection::instance().setServer(ip, serviceInfo.hostPort());
                    break;
                }
            };
        }
        break;
        default:
        break;
    }
}

esp::ServerConnection::ServerConnection() {
    this->tcp = new WiFiClient();
    this->serverIP = new IPAddress();
}

esp::ServerConnection::~ServerConnection() {
    // this will never happen (as long as this class is a singleton) and generates warnings.
    //delete this->tcp;
}

esp::ServerConnection& esp::ServerConnection::instance() {
    static esp::ServerConnection _instance;
    return _instance;
}

bool esp::ServerConnection::connected() {
    return this->tcp->connected();
}

void esp::ServerConnection::disconnect() {
    this->tcp->stop();
}

void esp::ServerConnection::setServer(IPAddress &ip, uint16_t port) {
    *(this->serverIP) = ip;
    this->serverPort = port;
}

void esp::ServerConnection::loop(bool isWiFiConnected) {
    if (!isWiFiConnected) {
        if (this->tcp->connected()) {
            this->tcp->stop();
        }
        return;
    }
    if (!hMDNSServiceQuery) {
        hMDNSServiceQuery = MDNS.installServiceQuery("ipocs", "tcp", MDNSServiceQueryCallback);
    }
    if (!this->tcp->connected()) {
        if (this->serverPort != 0 && *(this->serverIP) != IPAddress()) {
            //esp::Http::instance().log("Connecting to: " + this->serverIP.toString() + ":" + String(this->serverPort));
            char unitName[32];
            Configuration::getUnitName(unitName);
            byte returnVal = this->tcp->connect(*(this->serverIP), this->serverPort);
            if (returnVal == 1) {
                LedControl::instance().setState(ON);
                this->tcp->keepAlive(5, 1, 4);
                IPOCS::Message *msg = IPOCS::Message::create();
                msg->setObject(unitName);
                IPOCS::ConnectionRequestPacket *pkt = (IPOCS::ConnectionRequestPacket *)IPOCS::ConnectionRequestPacket::create();
                pkt->RM_PROTOCOL_VERSION = 0x0101;
                char sdVersion[10];
                sprintf(sdVersion, "%d", Configuration::getSiteDataCrc());
                pkt->RXID_SITE_DATA_VERSION = sdVersion;
                msg->setPacket(pkt);
                this->send(msg);
                delete msg;

                IPC::Message *ipcPing = IPC::Message::create();
                ipcPing->RT_TYPE = IPC::CESTAB;
                ipcPing->setPayload();
                esp::ArduinoConnection::instance().send(ipcPing);
                delete ipcPing;
            }
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

            char unitName[32];
            Configuration::getUnitName(unitName);
            if (strcmp(msg->getObject(), unitName) == 0) {
                if (msg->packet->RNID_PACKET == 5) {
                    IPOCS::ApplicationDataPacket* dataPkt = (IPOCS::ApplicationDataPacket* const)msg->packet;
                    uint8_t dataLength = dataPkt->RL_PACKET - 5;
                    Configuration::setSD(dataPkt->data, dataLength);
                    handleRestart(true);
                }
            } else {
                ArduinoConnection::instance().send(ipcMsg);
            }
            delete msg;
            delete ipcMsg;
        }
    }
}

void esp::ServerConnection::send(IPOCS::Message* msg) {
    if (!this->tcp->connected()) {
        //esp::Http::instance().log("Not connected, throwing message");
        return;
    }
    uint8_t buffer[255];
    uint8_t len = msg->serialize(buffer);
    this->tcp->write(buffer, len);
}