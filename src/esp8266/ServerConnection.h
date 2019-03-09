#ifndef ESP8266_SERVERCONNECTION_H
#define ESP8266_SERVERCONNECTION_H

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
            void disconnect();
        private:
            ServerConnection();
            virtual ~ServerConnection();
            WiFiClient* tcp;
    };
};

#endif