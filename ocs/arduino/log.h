#ifndef LOG_H
#define LOG_H

#ifdef _HAVE_HWSERIAL3
class String;
#define LOG(msg) Serial.println(String("= ") + String(msg)); Serial.flush();
static int depth = 0;
#else
#define LOG(msg) ;
#endif

class Log {
    public:
        Log(String message) {
#ifdef _HAVE_HWSERIAL3
            this->message = message;
            this->dd = ++depth;
            String toPrint;
            for (int i = 0; i < dd; i++) {
                toPrint += ">";
            }
            toPrint += " " + this->message;
            Serial.println(toPrint);
            Serial.flush();
#endif
        }
        ~Log() {
#ifdef _HAVE_HWSERIAL3
            depth--;
            String toPrint;
            for (int i = 0; i < dd; i++) {
                toPrint += "<";
            }
            toPrint += " " + this->message;
            Serial.println(toPrint);
            Serial.flush();
#endif
        }
    private:
#ifdef _HAVE_HWSERIAL3
        int dd;
        String message;
#endif
};

#endif
