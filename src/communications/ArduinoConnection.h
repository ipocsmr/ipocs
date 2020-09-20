#ifndef ARDUINOCONNECTION_H
#define ARDUINOCONNECTION_H

#include <stdint.h>
#include <stddef.h>

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
            void stop();
            void loop();
        private:
            ArduinoConnection() {}
            // Void pointer because the PacketSerial header
            // is not easy to do forward declarations for.
            void* packetSerial;
    };
};

#endif