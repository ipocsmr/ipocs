#ifndef LOG_H
#define LOG_H

class String;

#ifdef HAVE_HWSERIAL3
#define LOG_TRACE(msg) Log(msg);
#define LOG(msg) Serial.println(String("= ") + String(msg)); Serial.flush();
#else
#define LOG(msg) ;
#endif

class Log {
    public:
        Log(String message) {
            this->message = message;
            Serial.println("> " + message);
            Serial.flush();
        }
        ~Log() {
            Serial.println("< " + this->message);
            Serial.flush();
        }
    private:
        String message;
};

#endif
