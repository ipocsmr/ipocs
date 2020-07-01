#include "Message.h"
#include <uCRC16Lib.h>

/*******************************************************************
   Message
*/
IPC::Message::Message()
{
  this->RL_MESSAGE = 0;
  this->pld = new uint8_t[0];
}

IPC::Message* IPC::Message::create()
{
  return new IPC::Message();
}

IPC::Message* IPC::Message::create(const uint8_t buffer[])
{
  IPC::Message* msg = new IPC::Message();
  msg->RL_MESSAGE = buffer[2];
  msg->RT_TYPE = (IPC::RT_TYPE)buffer[3];
  msg->pld = new uint8_t[msg->RL_MESSAGE - 2];
  memcpy(msg->pld, buffer + 4, msg->RL_MESSAGE - 2);
  return msg;
}

bool IPC::Message::verifyChecksum(const uint8_t buffer[]) {
  uint16_t storedCrc = buffer[0] + (buffer[1] << 8);
  uint16_t calculatedCrc = uCRC16Lib::calculate(&buffer[2], 2);
  return storedCrc == calculatedCrc;
}

uint8_t IPC::Message::serialize(uint8_t buffer[])
{
  buffer[0] = 0x00;
  buffer[1] = 0x00;
  buffer[2] = this->RL_MESSAGE;
  buffer[3] = this->RT_TYPE;
  memcpy(buffer + 4, this->pld, this->RL_MESSAGE - 2);
  uint16_t calculatedCrc = uCRC16Lib::calculate(&buffer[2], 2);
  buffer[0] = calculatedCrc & 0xFF;
  buffer[1] = calculatedCrc >> 8;
  return this->RL_MESSAGE + 2;
}

void IPC::Message::setPayload(uint8_t* buffer, uint8_t length)
{
  if (this->pld != NULL) {
    delete (this->pld);
  }
  this->RL_MESSAGE = length + 2;
  this->pld = new uint8_t[length];
  memcpy(this->pld, buffer, length);
}

void IPC::Message::setPayload()
{
  if (this->pld != NULL) {
    delete (this->pld);
  }
  this->RL_MESSAGE = 2;
  this->pld = new uint8_t[0];
}

IPC::Message::~Message()
{
  if (this->pld != NULL) {
    delete (this->pld);
  }
}
