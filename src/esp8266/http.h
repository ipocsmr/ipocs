#ifndef HTTPHANDLER_H
#define HTTPHANDLER_H

#include <stdint.h>

class ESP8266WebServer;
class ESP8266HTTPUpdateServer;
class String;

namespace esp {
    class Http {
      public:
        static Http& instance();
        void setup();
        void loop();

        void index();
        void handleUnitIdUpdate();
        void handleSsidUpdate();
        void handlePwdUpdate();
        void handleSiteDataUpdate();
        void handleRestart(bool restartArduino);
        void handleNotFound();
        void log(const String& string);
      private:
        ~Http();
        Http();
        ESP8266WebServer* server;
        ESP8266HTTPUpdateServer* updateServer;
    };
}

#endif