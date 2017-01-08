/*****************
   Server Connection handling file.

   @author Fredrik Elestedt
   @date
   @history see the git log of the origin repository.
   @note For this to compile, you need https://github.com/per1234/EthernetMod
*/
#ifndef SERVER_CONNECTION_H
#define SERVER_CONNECTION_H

#include <Ethernet.h>

namespace IPOCS {
  class Message;
}

class ServerConnection {
  public:
    static ServerConnection& getInstance()
    {
      static ServerConnection instance;
      return instance;
    }

    void loadSaved();
    void loop();
    void setServer(IPAddress serverIP);

    void send(IPOCS::Message* msg);
  private:
    ServerConnection();
    ServerConnection(ServerConnection const&);              // Don't Implement
    void operator=(ServerConnection const&); // Don't implement

    EthernetClient server;
    long lastReconnect;
    IPAddress serverIP;
    String inputString;
};

#endif

