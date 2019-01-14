# gmjs_ocs
This project implements the IPOCS specification that can be found [here](https://github.com/GMJS/documentation).

## Hardware

This software is tested on both Arduino UNO and Arduino MEGA compatible devices
using an ESP8266 connected via serial port. The WeeESP8266 library is used to
control the ESP8266 module.

If the device has four serial ports (such as on a MEGA) it is assumed that the
fourth serial port is used to talk to the ESP8266 module.
If this is the case, serial debugging output will be enabled on the primary
serial port.