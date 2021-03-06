/**
 * Tiny and cross-device compatible CCITT CRC16 calculator library.
 *
 * @copyright Naguissa
 * @author Naguissa
 * @email naguissa@foroelectro.net
 * @version 1.0.0
 * @created 2018-04-21
 */
#ifndef _uCRC16Lib_
	#define _uCRC16Lib_

	#include <Arduino.h>

	#define uCRC16Lib_POLYNOMIAL 0x8408

	class uCRC16Lib {
		public:
			static uint16_t calculate(const uint8_t*, uint16_t);
			const static uint16_t crc_ok = 0x470F;

		private:
			// Static library, no need to construct objects
			uCRC16Lib();

	};

#endif

