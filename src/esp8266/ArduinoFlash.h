#ifndef ARDUINOFLASH_H
#define ARDUINOFLASH_H

#include <Arduino.h>

enum FlashProgress {
  stateInactive = 0,
  stateRestart,
  stateInit,
  stateSync,
  stateSyncAck,
  stateEnterProgMode,
  stateEnterProgModeAck,
  stateLoadAddress,
  stateLoadAddressAck,
  stateProgMem,
  stateProgMemAck,
  stateVerifyPage,
  stateVerifyPageAck,
  stateExitProgMode,
  stateExitProgModeAck,
};

namespace esp {
    class IntelHexParser;
    class ArduinoFlash {
        public:
            static ArduinoFlash& instance();
            static void verifyFile(String& fileName);
            void initiate(String& fileName, bool verify = false);

            void loop();
        private:
            ArduinoFlash();
            IntelHexParser* hexParser;
            enum FlashProgress progress;
            uint32_t ulStartOfFlashTime;
            bool verifyOnly;
            uint32_t ulLastLoop;
            uint32_t nextCommand;
    };
};

#endif
