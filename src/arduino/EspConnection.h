/*****************
   Server Connection handling file.

   @author Fredrik Elestedt
   @date
   @history see the git log of the origin repository.
   @note For this to compile, you need https://github.com/per1234/EthernetMod
*/
#ifndef ESP_CONNECTION_H
#define ESP_CONNECTION_H

#include <stdint.h>
#include <PacketSerial.h>

namespace IPC {
  class Message;
}

namespace ard {
  class EspConnection {
    public:
      static EspConnection& instance();

      void begin();
      void loop();
      
      void send(IPC::Message* msg, bool print = true);

      unsigned long m_ulLastMsg = 0;
    private:
      EspConnection() {}
      SLIPPacketSerial* packetSerial = nullptr;
  };
};

#endif
