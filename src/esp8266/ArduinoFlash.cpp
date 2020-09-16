#include "ArduinoFlash.h"
#include "../communications/ArduinoConnection.h"
#include "../IPC/Message.h"
#include "http.h"
#include "stk.h"
#include "IntelHexParser.h"

#define INIT_DELAY 150

esp::ArduinoFlash& esp::ArduinoFlash::instance() {
  static esp::ArduinoFlash af;
  return af;
}

esp::ArduinoFlash::ArduinoFlash(): progress(stateInactive) {
  this->hexParser = new IntelHexParser();
  this->ulLastLoop = millis();
  this->nextCommand = 0U;
}

void esp::ArduinoFlash::initiate(String& fileName, bool verify) {
  esp::Http::instance().log("Initiating flash procedure...");
  ulStartOfFlashTime = millis();
  this->hexParser->Open(fileName);
  progress = stateInit;
  this->verifyOnly = verify;
  esp::Http::instance().broadcastOpInPorgress(this->isBusy());
  this->nextCommand = 10U;
}

void esp::ArduinoFlash::verifyFile(String& fileName, std::function<void(uint32_t, uint32_t)> cb) {
  esp::Http::instance().log("Testing loading of file " + fileName);
  IntelHexParser* hexParser = new IntelHexParser();
  hexParser->Open(fileName);
  cb(hexParser->position(), hexParser->size());
  while (hexParser->EnsurePage() >= 0 && !hexParser->data.empty()) {
    String printData;
    for (uint8_t i = 0; i < PAGE_SIZE; i++)
    {
      char sendBuffer = 0xFF;
      if (!hexParser->data.empty())
      {
        sendBuffer = hexParser->data.front();
        hexParser->data.erase(hexParser->data.begin());
      }
      printData += hexParser->padString(String(sendBuffer, HEX));
    }
    esp::Http::instance().log("FullPag: " + hexParser->padString(String(hexParser->address, HEX), 4) + ", Data: " + printData);
    hexParser->address += PAGE_SIZE;
    if (hexParser->position() <= hexParser->size()) {
      cb(hexParser->position(), hexParser->size());
    }
  }
  cb(1, 1);
  esp::Http::instance().log("DONE Testing loading of file " + fileName);
  hexParser->Close();
  delete hexParser;
}

void esp::ArduinoFlash::loop() {
  ESP.wdtFeed();
  uint32_t ulMillis = millis();
  if (this->nextCommand == 0)
  {
    this->ulLastLoop = ulMillis;
    return;
  }
  if (ulMillis - this->ulLastLoop < this->nextCommand)
  {
    return;
  }
  this->ulLastLoop = ulMillis;
  this->nextCommand = 0;
  switch (progress) {
    case stateInit: {
      esp::Http::instance().log("Restarting arduino...");
      esp::Http::instance().broadcastMessage(esp::Http::PROGRESS_MESSAGE, "Restarting arduino...");
      progress = stateRestart;
      IPC::Message* ipcMsg = IPC::Message::create();
      ipcMsg->RT_TYPE = IPC::RESTART;
      ipcMsg->setPayload();
      esp::ArduinoConnection::instance().send(ipcMsg);
      delete ipcMsg;
      this->nextCommand = 150U;
    break; }
    case stateRestart: {
      esp::ArduinoConnection::instance().stop();
      progress = stateSync;
      ulStartOfFlashTime = millis();
      this->nextCommand = 50U;
    break; }
    case stateSync: {
      while (Serial.available())
      {
        esp::Http::instance().log("Read unexpected char: 0x" + this->hexParser->padString(String(Serial.read(), 16)));
      }
      if (millis() - ulStartOfFlashTime > 1000) {
        esp::Http::instance().broadcastMessage(esp::Http::PROGRESS_MESSAGE, "Sync with arduino bootloader failed, aborting...");
        esp::Http::instance().log("Operation timeout, aborting...");
        esp::Http::instance().broadcastMessage(esp::Http::ERROR, "Operation failed...");
        progress = stateAbort;
      } else {
        esp::Http::instance().log("Trying to sync with optiboot...");
        Serial.write(STK_GET_SYNC);
        Serial.write(CRC_EOP);
        progress = stateSyncAck;
      }
      this->nextCommand = 10U;
    break; }
    case stateSyncAck: {
      uint8_t buffer[2] = { 0x00, 0x00 };
      size_t length = Serial.readBytes(buffer, 2);
      esp::Http::instance().log("Length " + String(length));
      if (length != 2) {
        progress = stateSync;
        this->nextCommand = 10U;
        return;
      }
      if (buffer[0] == STK_INSYNC && buffer[1] == STK_OK) {
        esp::Http::instance().broadcastMessage(esp::Http::PROGRESS_MESSAGE, "Starting operation...");
        esp::Http::instance().log("Arduino reports in sync... proceeding");
        int8_t result = 1;
        while (this->hexParser->data.size() < PAGE_SIZE && result > 0)
        {
          result = this->hexParser->EnsurePage();
          if (result == -1)
          {
            esp::Http::instance().broadcastMessage(esp::Http::ERROR, "Operation failed...");
            esp::Http::instance().log("Parsing failed. Aborting prog mode");
            progress = stateExitProgMode;
            this->nextCommand = 10U;
            return;
          }
        }
        progress = stateEnterProgMode;
      } else {
        esp::Http::instance().broadcastMessage(esp::Http::ERROR, "Operation failed...");
        esp::Http::instance().log("Got somthing else...");
      }
      this->nextCommand = 10U;
    break; }
    case stateEnterProgMode: {
      Serial.write(STK_ENTER_PROGMODE);
      Serial.write(CRC_EOP);
      progress = stateEnterProgModeAck;
      this->nextCommand = 50U;
    break; }
    case stateEnterProgModeAck: {
      uint8_t buffer[2] = { 0x00, 0x00 };
      size_t length = Serial.readBytes(buffer, 2);
      if (length == 2 && buffer[0] == STK_INSYNC && buffer[1] == STK_OK) {
        progress = stateLoadAddress;
        if (this->verifyOnly) {
          esp::Http::instance().broadcastMessage(esp::Http::PROGRESS_MESSAGE, "Verifying...");
        } else {
          esp::Http::instance().broadcastMessage(esp::Http::PROGRESS_MESSAGE, "Programming...");
        }
      } else {
        esp::Http::instance().broadcastMessage(esp::Http::ERROR, "Operation failed...");
        esp::Http::instance().log("Got Unexpexted value " + String(buffer[0], 16) + "," + String(buffer[1], 16));
        progress = stateExitProgMode;
      }
      this->nextCommand = 10U;
    break; }
    case stateLoadAddress: {
      while (Serial.available())
      {
        esp::Http::instance().log("Read unexpected char: 0x" + this->hexParser->padString(String(Serial.read(), 16)));
      }
      uint16_t addr = this->hexParser->address >> 1; // word address
      uint8_t sendBuffer[4] = { STK_LOAD_ADDRESS, (uint8_t)(addr & 0xFF), (uint8_t)((addr >> 8) & 0xFF), CRC_EOP };
      Serial.write(sendBuffer, 4);
      progress = stateLoadAddressAck;
      this->nextCommand = 10U;
    break; }
    case stateLoadAddressAck: {
      uint8_t buffer[2] = { 0x00, 0x00 };
      size_t length = Serial.readBytes(buffer, 2);
      if (length == 2 && buffer[0] == STK_INSYNC && buffer[1] == STK_OK) {
        progress = this->verifyOnly ? stateVerifyPage : stateProgMem;
      } else {
        esp::Http::instance().broadcastMessage(esp::Http::ERROR, "Operation failed...");
        esp::Http::instance().log("Got Unexpexted value " + String(buffer[0], 16) + "," + String(buffer[1], 16));
        progress = stateExitProgMode;
      }
      this->nextCommand = 10U;
    break; }
    case stateProgMem: {
      while (Serial.available())
      {
        esp::Http::instance().log("Read unexpected char: 0x" + this->hexParser->padString(String(Serial.read(), 16)));
      }
      uint8_t sendBuffer[PAGE_SIZE + 5];
      memset(sendBuffer, 0xFF, PAGE_SIZE);
      sendBuffer[0] = STK_PROG_PAGE;
      sendBuffer[1] = PAGE_SIZE >> 8;
      sendBuffer[2] = PAGE_SIZE & 0xFF;
      sendBuffer[3] = 'F';
      String printData;
      for (uint8_t i = 0; i < PAGE_SIZE; i++)
      {
        if (!hexParser->data.empty())
        {
          sendBuffer[4 + i] = this->hexParser->data.front();
          printData += hexParser->padString(String(sendBuffer[4 + i], 16));
          this->hexParser->data.erase(this->hexParser->data.begin());
        }
      }
      sendBuffer[4 + PAGE_SIZE] = CRC_EOP;
      esp::Http::instance().log("ProgMem: " + this->hexParser->padString(String(this->hexParser->address, HEX), 4) + ", Data: " + printData);
      this->hexParser->address += PAGE_SIZE;
      Serial.write(sendBuffer, 5 + PAGE_SIZE);
      progress = stateProgMemAck;
      this->nextCommand = 200U;
    break; }
    case stateProgMemAck: {
      char buffer[2] = { 0x00, 0x00 };
      size_t length = Serial.readBytes(buffer, 2);
      if (length == 2 && buffer[0] == STK_INSYNC && buffer[1] == STK_OK) {
        int32_t result = 1;
        while (this->hexParser->data.size() < PAGE_SIZE && result > 0)
        {
          result = this->hexParser->EnsurePage();
          if (result == -1)
          {
            esp::Http::instance().broadcastMessage(esp::Http::ERROR, "Operation failed...");
            esp::Http::instance().broadcastMessage(esp::Http::PROGRESS_MESSAGE, "Failed parsing");
            esp::Http::instance().log("Parsing failed. Aborting prog mode");
            progress = stateExitProgMode;
            this->nextCommand = 10U;
            return;
          }
        }
        uint32_t progPercent = (uint32_t)(((float)this->hexParser->position() / this->hexParser->size()) * 100);
        String strProgress = String(progPercent <= 100 ? progPercent : 100);
        esp::Http::instance().broadcastMessage(esp::Http::PROGRESS, strProgress.c_str());
        if (this->hexParser->data.size() == 0) {
          esp::Http::instance().broadcastMessage(esp::Http::SUCCESS, "Programming successful...");
        }
        progress = this->hexParser->data.size() == 0 ? stateExitProgMode : stateLoadAddress;
      } else {
        esp::Http::instance().log("Available Bytes: " + String(Serial.available()) + " Got Unexpexted value " + String(buffer[0], 16) + "," + String(buffer[1], 16));
        progress = stateExitProgMode;
      }
      this->nextCommand = 10U;
    break; }
    case stateVerifyPage: {
      while (Serial.available())
      {
        esp::Http::instance().log("Read unexpected char: 0x" + this->hexParser->padString(String(Serial.read(), 16)));
      }
      Serial.write(STK_READ_PAGE);
      Serial.write((char)(PAGE_SIZE >> 8));
      Serial.write((char)(PAGE_SIZE & 0xFF));
      Serial.write('F');
      Serial.write(CRC_EOP);
      progress = stateVerifyPageAck;
      this->nextCommand = 200U;
    break; }
    case stateVerifyPageAck: {
      char readBuffer[PAGE_SIZE + 2];
      memset(readBuffer, 0xFF, PAGE_SIZE + 2);
      size_t length = Serial.readBytes(readBuffer, 2 + PAGE_SIZE);
      if (length == 2 + PAGE_SIZE && readBuffer[0] == STK_INSYNC && readBuffer[1 + PAGE_SIZE] == STK_OK) {
        bool verified = true;
        String printData;
        for (uint32_t i = 0; i < PAGE_SIZE; i++)
        {
          printData += hexParser->padString(String(readBuffer[1 + i], HEX));
          verified = verified && readBuffer[1 + i] == this->hexParser->data.front();
          this->hexParser->data.erase(this->hexParser->data.begin());
          if (this->hexParser->data.empty())
          {
            break;
          }
        }
        esp::Http::instance().log("Verifie: " + this->hexParser->padString(String(this->hexParser->address, HEX), 4) + ", Data: " + printData);
        this->hexParser->address += PAGE_SIZE;
        if (!verified)
        {
          esp::Http::instance().broadcastMessage(esp::Http::ERROR, "Verification failed...");
          esp::Http::instance().log("Unable to verify page, exiting!");
        }
        else
        {
          int32_t result = 1;
          while (this->hexParser->data.size() < PAGE_SIZE && result > 0)
          {
            result = this->hexParser->EnsurePage();
            if (result == -1)
            {
              esp::Http::instance().broadcastMessage(esp::Http::ERROR, "Operation failed...");
              esp::Http::instance().log("Parsing failed. Aborting prog mode");
              progress = stateExitProgMode;
              this->nextCommand = 10U;
              return;
            }
          }
          if (this->hexParser->data.size() == 0 && verified)
          {
            esp::Http::instance().broadcastMessage(esp::Http::SUCCESS, "Verification successful...");
            esp::Http::instance().log("Verification successful");
          }
          uint32_t progPercent = (uint32_t)(((float)this->hexParser->position() / this->hexParser->size()) * 100);
          String strProgress = String(progPercent <= 100 ? progPercent : 100);
          esp::Http::instance().broadcastMessage(esp::Http::PROGRESS, strProgress.c_str());
          progress = this->hexParser->data.size() == 0 ? stateExitProgMode : stateLoadAddress;
        }
        
      } else {
        esp::Http::instance().log("Available " + String(Serial.available()));
        esp::Http::instance().log("Length did maybe not match " + String(length) + " ?= " + String(2 + PAGE_SIZE));
        esp::Http::instance().log("Got Unexpexted value " + String(readBuffer[0], HEX) + "," + String(readBuffer[1], HEX));
        progress = stateExitProgMode;
      }
      this->nextCommand = 10U;
    break; }
    case stateExitProgMode: {
      Serial.write(STK_LEAVE_PROGMODE);
      Serial.write(CRC_EOP);
      progress = stateInactive;
      esp::Http::instance().log("Programming done rebooting arduino...");
      esp::Http::instance().broadcastMessage(esp::Http::PROGRESS_MESSAGE, "Operation done...");
      esp::Http::instance().broadcastOpInPorgress(this->isBusy());
      this->hexParser->Close();
      this->nextCommand = 10U;
    break; }
    case stateExitProgModeAck: {
      char buffer[2] = { 0x00, 0x00 };
      size_t length = Serial.readBytes(buffer, 2);
      if (length == 2 && buffer[0] == STK_INSYNC && buffer[1] == STK_OK) {
        progress = stateInactive;
      } else {
        esp::Http::instance().log("Got Unexpexted value " + String(buffer[0], HEX) + "," + String(buffer[1], HEX));
        progress = stateExitProgMode;
      }
      this->nextCommand = 2000U;
    break; }
    case stateAbort: {
      progress = stateInactive;
      esp::Http::instance().broadcastOpInPorgress(this->isBusy());
      this->nextCommand = 1U;
    }
    case stateInactive: {
      esp::ArduinoConnection::instance().begin();
    break; }
  }
}
