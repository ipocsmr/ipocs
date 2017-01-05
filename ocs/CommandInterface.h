/*****************
 * Command Interface - definition file
 * 
 * Handles USB commands for the software
 * 
 * @author Fredrik Elestedt
 * @date
 * @history see the git log of the origin repository.
 * 
 * Storage usage:
 * 
 * Bytes - Usage
 * 0-1   - Unit ID
 * 2-18  - IPv4 (or in future IPv6 address)
 * 19-   - Object configuration data
 */
#ifndef COMMAND_INTERFACE_H
#define COMMAND_INTERFACE_H

#include <Arduino.h>

class CommandInterface {
  public:
    static CommandInterface& getInstance()
    {
      static CommandInterface instance;
      return instance;
    }
  
    void setup();
    void loop();
  private:
    void setupMode();
    bool handleCommand(String command);
};

#endif

