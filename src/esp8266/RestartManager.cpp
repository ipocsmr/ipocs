#include "../communications/RestartManager.h"
#include "../communications/ArduinoConnection.h"
#include "../IPC/Message.h"
#include "http.h"

void handleRestart(bool restartArduino) {
  if (!restartArduino) {
    ESP.restart();
  } else {
    esp::Http::instance().log("Resetting arduino");
    IPC::Message *message = IPC::Message::create();
    message->RT_TYPE = IPC::RESTART;
    message->setPayload();
    esp::ArduinoConnection::instance().send(message);
    delete message;
  }
}
