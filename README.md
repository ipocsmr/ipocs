# gmjs_ocs
This project implements the IPOCS specification that can be found [here](https://github.com/GMJS/documentation).

## Hardware

### Arduino

#### Uno

Untested, but should work.

#### Mega

Fully tested. ESP must be connected to serial port 3.

### ESP8266

Implements communication with stationary. Talks to Arduino over serial line.

## Installation

You need to have Platform IO installed.
To compile and upload to Arduino, run

```
platformio run -e mega -t upload
```

This will automatically upload to the first available Arduino board.
The `-e` option expects a value from `platformio.ini`.  For ESP8266 and serial upload, use `esp8266_serial`.
If you just wand to build, remove `-t upload` from the command.

Once the ESP has been flashed once, subsequent flashed can be done over WiFi.

## Setting up

The ESP will try to connect to a configured WiFi (even if none has been configured).
Once it has failed 4 times it will set up a SoftAP and respond on IP 192.168.4.1.
Use this to make initial configuration

DIP 5+6+7 ON, all others OFF
Command to upload to ESP using cable> platformio run -e esp8266_serial -t upload
DIP 3+4 ON, all others OFF
Command to upload to uno using cable> platformio run -e uno -t upload
When done set DIP 1+2 ON, all others OFF 