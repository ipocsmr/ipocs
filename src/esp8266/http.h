#ifndef HTTPHANDLER_H
#define HTTPHANDLER_H

#include <stdint.h>
#include <WebSockets.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <ESP8266HTTPUpdateServer.h>

class String;

namespace esp {
    class Http {
      public:
        static Http& instance();
        void setup();
        void loop();

        void index();
        void aIndex();
        void handleReason();
        void handleUnitIdUpdate();
        void handleSsidUpdate();
        void handlePwdUpdate();
        void handleApplyWiFi();
        void handleSiteDataUpdate();
        void handleRestart(bool restartArduino);
        void handleNotFound();
        void handleFileDelete();
        void handleFileUpload();
        void handleArduinoFlash(bool verifyOnly);
        void handleVerifyFile();
        void log(const String& string);
        void setArduinoVersion(uint8_t* versionBuffer);
      private:
        ~Http();
        Http();
        WebSocketsServer* webSocket;
        ESP8266WebServer* server;
        ESP8266HTTPUpdateServer* updateServer;
        char* arduinoVersion;
        void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length);
    };
}

#endif