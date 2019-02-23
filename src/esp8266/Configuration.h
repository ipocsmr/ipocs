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
 * 2-35  - SSID if wifi shield is present (33 chars including null)
 * 36-97 - SSID password (61 characters including null)
 * 98-99 - Object configuration data CCIT Checksum
 * 100-  - Object configuration data
 */
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <stddef.h>
#include <stdint.h>

class Configuration {
  public:
    static uint16_t getUnitID();
    static void setUnitID(uint16_t unitID);
    static size_t getSSID(char ssid[]);
    static void setSSID(const char ssid[]);
    static size_t getPassword(char pwd[]);
    static void setPassword(const char pwd[]);
    static uint8_t getSD(uint8_t data[], int dataLength);
    static void setSD(uint8_t data[], int dataLength);

    static uint16_t getSiteDataCrc();
    static bool verifyCrc();
};

#endif