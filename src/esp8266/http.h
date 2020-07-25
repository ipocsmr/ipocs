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
        static constexpr const char* const SUCCESS = "success";
        static constexpr const char* const ERROR = "error";
        static constexpr const char* const PROGRESS = "progress";
        static constexpr const char* const PROGRESS_MESSAGE = "progressMessage";
        static Http& instance();
        void setup();
        void loop();

        void index();
        void aIndex();
        void handleReason();
        void handleRestart(bool restartArduino);
        void handleNotFound();
        void handleFileUpload();
        void log(const String& string);
        void setArduinoVersion(uint8_t* versionBuffer);
        void broadcastOpInPorgress(bool isBusy);
        void broadcastMessage(const char* action, const char* value);
      private:
        ~Http();
        Http();
        WebSocketsServer* webSocket;
        ESP8266WebServer* server;
        ESP8266HTTPUpdateServer* updateServer;
        char* arduinoVersion;
        void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length);

        String getJsonFormatted(const char* const action, String value);
        String getJsonFormatted(String action, String value);
        String getUnitId();
        String getSsid();
        String getPwd();
        String getSiteData();
        String getFiles();
    };
}

#endif