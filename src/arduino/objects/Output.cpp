
#include "Output.h"
#include "../ObjectStore.h"
#include "../EspConnection.h"
#include "../../IPC/Message.h"
#include "../../IPOCS/Message.h"
#include "../../IPOCS/Packets/SetOutputPacket.h"

Output::Output(): lastOrderTime(0), offDelay(0), outputPin(0),
                  lastSentState(IPOCS::OutputStatusPacket::E_RQ_OUTPUT_STATE::OFF) {
}

void Output::handleOrder(IPOCS::Packet* basePacket)
{
  if (basePacket->RNID_PACKET == 13) {
    IPOCS::SetOutputPacket* packet = (IPOCS::SetOutputPacket*)basePacket;
    // Set output high
    this->lastOrderTime = millis();
    ard::EspConnection::instance().log("+ Set Output, "
      "state=" + String(packet->RQ_OUTPUT_COMMAND == IPOCS::SetOutputPacket::E_RQ_OUTPUT_COMMAND::ON ? "ON" : "OFF") + ", "
      "duration=" + String(packet->RT_DURATION)
    );
    if (packet->RQ_OUTPUT_COMMAND == IPOCS::SetOutputPacket::E_RQ_OUTPUT_COMMAND::ON) {
      this->offDelay = packet->RT_DURATION;
      digitalWrite(this->outputPin, HIGH);
      if (this->lastSentState == IPOCS::OutputStatusPacket::E_RQ_OUTPUT_STATE::OFF) {
        this->lastSentState = IPOCS::OutputStatusPacket::E_RQ_OUTPUT_STATE::ON;
        this->sendStatus();
      }
    } else {
      this->offDelay = 0;
      digitalWrite(this->outputPin, LOW);
      if (this->lastSentState == IPOCS::OutputStatusPacket::E_RQ_OUTPUT_STATE::ON) {
        this->lastSentState = IPOCS::OutputStatusPacket::E_RQ_OUTPUT_STATE::OFF;
        this->sendStatus();
      }
    }
  } else if (basePacket->RNID_PACKET == 7) {
    this->sendStatus();
  } else {
    // TODO: Send alarm or something about invalid packet type.
  }
}

IPOCS::Packet* Output::getStatusPacket() {
  IPOCS::OutputStatusPacket* pkt = (IPOCS::OutputStatusPacket*)IPOCS::OutputStatusPacket::create();
  pkt->RQ_OUTPUT_STATE = this->lastSentState;
  return pkt;
}

void Output::loop() {
  if ((offDelay != 0) && (millis() - this->lastOrderTime) >= this->offDelay * 100) {
    ard::EspConnection::instance().log("- Set Output, "
      "duration expired, "
      "delay=" + String(millis() - this->lastOrderTime)
    );
    this->offDelay = 0;
    digitalWrite(this->outputPin, LOW);
    if (this->lastSentState == IPOCS::OutputStatusPacket::E_RQ_OUTPUT_STATE::ON) {
      this->lastSentState = IPOCS::OutputStatusPacket::E_RQ_OUTPUT_STATE::OFF;
      this->sendStatus();
    }
  }
}

void Output::objectInit(const uint8_t configData[], int configDataLen)
{
  this->outputPin = configData[0] + 1;

  pinMode(this->outputPin, OUTPUT);
  digitalWrite(this->outputPin, LOW);

  ard::EspConnection::instance().log("> Output, "
    "outputPin=" + String(this->outputPin)
  );
}

static BasicObject* createOutput()
{
  return new Output();
}

__attribute__((constructor))
static void initialize_output() {
  ObjectStore::getInstance().registerType(10, &createOutput);
}
