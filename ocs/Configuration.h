/*****************
 * Configuration - definition file.
 * 
 * Handles memory storage
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
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <Ethernet.h>

class Configuration {
  public:
    static unsigned int getUnitID();
    static void setUnitID(unsigned int unitID);
    static IPAddress getServer();
    static void setServer(IPAddress newIP);
};

#endif

