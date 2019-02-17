#ifndef LOG_H
#define LOG_H

class String;

#ifdef HAVE_HWSERIAL3
#define LOG(msg) Serial.println(String("= ") + String(msg)); Serial.flush();
#define LOGN(msg) Serial.print(String(msg)); Serial.flush();
#else
#define LOG(msg) ;
#define LOGN(msg) ;
#endif

class Log {
    public:
        Log(String message) {
#ifdef HAVE_HWSERIAL3
            this->message = message;
            Serial.println("> " + message);
            Serial.flush();
#endif
        }
        ~Log() {
#ifdef HAVE_HWSERIAL3
            Serial.println("< " + this->message);
            Serial.flush();
#endif
        }
    private:
#ifdef HAVE_HWSERIAL3
        String message;
#endif
};

#endif
