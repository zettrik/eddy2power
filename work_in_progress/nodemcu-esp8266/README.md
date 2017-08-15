# micropython firmware
* http://micropython.org/download/
* http://docs.micropython.org/en/latest/esp8266/

## install esptool 
* aptitude install python3-venv
* python3 -m venv testproject
* source testproject/bin/activate
* pip install esptool
* pip freeze > requirements.txt
 * for later use with: pip install -r requirements.txt

* deactivate

## flash firmware with esptool 
* esptool.py --chip esp8266 --port /dev/ttyUSB0 erase_flash 
* esptool.py --chip esp8266 --port /dev/ttyUSB0 --baud 460800 write_flash -fm dio 0 firmware.bin
 * "-fm dio" is important for nodemcu boards!

## micropython benützen
### connect to serial console
* screen /dev/ttyUSB0 115200
* help()

Pressing ctrl-E will enter a special paste mode. This allows you to copy and
paste a chunk of text into the REPL. If you press ctrl-E you will see the
paste-mode prompt.

### wifi
Initially, the device configures itself as a WiFi access point (AP).

ESSID: MicroPython-xxxxxx (x’s are replaced with part of the MAC address).
Password: micropythoN (note the upper-case N).
IP address of the board: 192.168.4.1.
DHCP-server is activated.

### development with ampy
* venv pip install adafruit-ampy
* ampy -p /dev/ttyUSB0 get boot.py


## pythonscripts

{{{
  import machine
  pin = machine.Pin(2, machine.Pin.OUT)
  pin.on()
  pin.off()
}}}


## handle sd cards
* https://nodemcu.readthedocs.io/en/master/en/sdcard/ -- sd card gpio connection
* http://www.instructables.com/id/Cheap-DIY-SD-card-breadboard-socket/
 * build with micro sd card adapter
* https://github.com/lvidarte/esp8266/tree/master/examples/sdcard

## mcp3208
connect 12 bit a/d 
* https://github.com/Sillern/esp_mcp3008
* micropython mcp?


----
# nodemcu firmware
instead of micropython you can also use lua environment

## build firmware for your needs
* https://nodemcu-build.com/index.php

## flash firmware with esptool
* esptool -cp /dev/ttyUSB0 -cd nodemcu -cb 9600 -cf firmware-integer.bin

## develop & upload lua scripts
### esplorer
* https://esp8266.ru/esplorer/ -- IDE and upload tool
 * https://github.com/4refr0nt/luatool -- cli upload

### nodemcu-uploader
* pip install nodemcu-uploader
* nodemcu-uploader -B 9600 terminal
 * firmware before mid 2016 used 9600 instead of 115200 now

### arduino sdk - programming ide
 * http://www.instructables.com/id/Programming-the-ESP8266-12E-using-Arduino-software/

### espressif
* https://frightanic.com/iot/tools-ides-nodemcu/

----
## links
* https://github.com/nodemcu
* https://frightanic.com/iot/tools-ides-nodemcu/ -- comparison of tools
* https://forum.micropython.org/viewtopic.php?t=2240 -- EDIT-DEPLOY-RUN-TEST cycle
* http://www.instructables.com/id/ESP8266-Li-Ion-Battery-rechargeable-battery-power-/
