/*****************
 * Command Interface - definition file
 * 
 * Handles USB commands for the software
 * 
 * @author Fredrik Elestedt
 * @date
 * @history see the git log of the origin repository.
 * 
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

