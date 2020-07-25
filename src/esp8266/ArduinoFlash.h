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
  stateAbort,
};

namespace esp {
    class IntelHexParser;
    class ArduinoFlash {
        public:
            static ArduinoFlash& instance();
            static void verifyFile(String& fileName, std::function<void(uint32_t, uint32_t)> cb);
            void initiate(String& fileName, bool verify = false);

            bool isBusy() { return this->progress != stateInactive; }

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
