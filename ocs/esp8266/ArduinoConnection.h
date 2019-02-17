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

            void begin();
            void loop();

            void log(const String& msg);
        private:
            ArduinoConnection() {}
            SLIPPacketSerial* packetSerial;
    };
};

#endif