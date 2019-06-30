#ifndef ARDUINOCONNECTION_H
#define ARDUINOCONNECTION_H

#include <PacketSerial.h>

namespace IPC {
    class Message;
}

namespace esp {
    class ArduinoConnection {
        public:
            static ArduinoConnection& instance();
            void send(IPC::Message* msg);
            void send(const uint8_t* const msg, const size_t length);
            void send(const char* const msg, const size_t length);

            void begin();
            void loop();
            unsigned long m_ulLastMsg = 0;
        private:
            ArduinoConnection() {}
            SLIPPacketSerial* packetSerial;
    };
};

#endif