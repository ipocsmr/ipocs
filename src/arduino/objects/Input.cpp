#include "Input.h"
#include "../ObjectStore.h"
#include "../../IPOCS/Packets/InputStatusPacket.h"
#include <Arduino.h>
#include "../EspConnection.h"

Input::Input(): inputPin(0), debounceTime(0), releaseHoldTime(0), changeFirstRegisteredTime(0),
                  lastSentState(IPOCS::InputStatusPacket::E_RQ_INPUT_STATE::OFF) {
}

void Input::handleOrder(IPOCS::Packet* basePacket)
{
  if (basePacket->RNID_PACKET == 7) {
    this->sendStatus();
  } else {
    // TODO: Send alarm or something about invalid packet type.
  }
}

IPOCS::Packet* Input::getStatusPacket() {
  IPOCS::InputStatusPacket* pkt = (IPOCS::InputStatusPacket*)IPOCS::InputStatusPacket::create();
  pkt->RQ_INPUT_STATE = this->lastSentState;
  return pkt;
}

void Input::loop() {
  int data = digitalRead(this->inputPin);
  if (lastSentState == IPOCS::InputStatusPacket::E_RQ_INPUT_STATE::OFF) {
    if (data == LOW) {
      uint16_t cTime = millis();
      if (changeFirstRegisteredTime == 0) {
        changeFirstRegisteredTime = cTime;
      }
      if ((cTime - changeFirstRegisteredTime) >= this->debounceTime) {
        changeFirstRegisteredTime = 0;
        lastSentState = IPOCS::InputStatusPacket::E_RQ_INPUT_STATE::ON;
        this->sendStatus();
      }
    } else {
      changeFirstRegisteredTime = 0;
    }
  } else if (lastSentState == IPOCS::InputStatusPacket::E_RQ_INPUT_STATE::ON) {
    if (data == HIGH) {
      uint16_t cTime = millis();
      if (changeFirstRegisteredTime == 0) {
        changeFirstRegisteredTime = cTime;
      }
      if ((cTime - changeFirstRegisteredTime) >= (this->releaseHoldTime * 100)) {
        changeFirstRegisteredTime = 0;
        lastSentState = IPOCS::InputStatusPacket::E_RQ_INPUT_STATE::OFF;
        this->sendStatus();
      }
    } else {
      changeFirstRegisteredTime = 0;
    }
  }
}

void Input::objectInit(const uint8_t configData[], int configDataLen)
{
  this->inputPin = configData[0] + 1;
  this->debounceTime = configData[1];
  this->releaseHoldTime = configData[2];

  ard::EspConnection::instance().log("> Input (" + String(objectName) + "), "
    "ip=" + String(this->inputPin) + ", "
    "dt=" + String(this->debounceTime) + ", "
    "rht=" + String(this->releaseHoldTime)
  );
  pinMode(this->inputPin, INPUT_PULLUP);
}

static BasicObject* createInput()
{
  return new Input();
}

__attribute__((constructor))
static void initialize_input() {
  ObjectStore::getInstance().registerType(11, &createInput);
}
