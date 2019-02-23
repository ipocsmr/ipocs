#include "uCRC16Lib.h"

/**
 * Constructor
 *
 * Nothing to do here
 */
uCRC16Lib::uCRC16Lib() {}


/**
 * Calculate CRC16 function
 *
 * @param	data_p	*char	Pointer to data
 * @param	length	uint16_t	Length, in bytes, of data to calculate CRC16 of. Should be the same or inferior to data pointer's length.
 */
uint16_t uCRC16Lib::calculate(const uint8_t *data_p, uint16_t length) {
   int  crc;
   char i;
   crc = 0xFFFF;
   do {
      crc = crc ^ (int) *data_p++ << 8;
      i = 8;
      do {
         if (crc & 0x8000)
            crc = crc << 1 ^ 0x1021;
         else
            crc = crc << 1;
      } while(--i);
   } while (--length > 0);
   return (crc);
}
