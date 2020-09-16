#ifndef ESP8266_SERVERCONNECTION_H
#define ESP8266_SERVERCONNECTION_H

#include <ESP8266WiFi.h>

class WiFiClient;
namespace IPOCS {
    class Message;
};

namespace esp {
    class ServerConnection {
        public:
            static ServerConnection& instance();
            void loop(bool isWiFiConnected);
            void send(IPOCS::Message* msg);
            void setServer(IPAddress& ip, uint16_t port);
            void disconnect();
            bool connected();
        private:
            ServerConnection();
            virtual ~ServerConnection();
            WiFiClient* tcp;
            IPAddress serverIP;
            uint16_t serverPort;
    };
};

#endif