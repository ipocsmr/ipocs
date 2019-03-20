#ifndef LOG_H
#define LOG_H

#ifdef _HAVE_HWSERIAL1
class String;
#define LOG(msg) Serial1.println(String("= ") + String(msg)); Serial1.flush();
static int depth = 0;
#else
#define LOG(msg) ;
#endif

class Log {
    public:
        Log(String message) {
#ifdef _HAVE_HWSERIAL1
            this->message = message;
            this->dd = ++depth;
            String toPrint;
            for (int i = 0; i < dd; i++) {
                toPrint += ">";
            }
            toPrint += " " + this->message;
            Serial1.println(toPrint);
            Serial1.flush();
#endif
        }
        ~Log() {
#ifdef _HAVE_HWSERIAL1
            depth--;
            String toPrint;
            for (int i = 0; i < dd; i++) {
                toPrint += "<";
            }
            toPrint += " " + this->message;
            Serial1.println(toPrint);
            Serial1.flush();
#endif
        }
    private:
#ifdef _HAVE_HWSERIAL1
        int dd;
        String message;
#endif
};

#endif
