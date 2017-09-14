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

```
  import machine
  pin = machine.Pin(2, machine.Pin.OUT)
  pin.on()
  pin.off()
```

## handle sd cards
* build with micro sd card adapter
 * http://www.instructables.com/id/Cheap-DIY-SD-card-breadboard-socket/
* https://learn.adafruit.com/micropython-hardware-sd-cards/esp8266
* https://forum.micropython.org/viewtopic.php?f=16&t=3404
* wired connections:
| SD Card Pin | NodeMCU Pin (GPIO) |
| ---- | ---- |
| 1 CS   | D8 (GPIO15) |
| 2 DIN, MOSI  | D7 (GPIO13) |
| 3 GND  | GND |
| 4 VCC  | 3,3V |
| 5 CLK  | D5 (GPIO14) |
| 6 GND  | GND |
| 7 DOUT, MISO | D6 (GPIO12) |
* put sdcard.py in micropython environment and use like in sd.py
 * https://github.com/micropython/micropython/blob/master/drivers/sdcard/sdcard.py

## mcp3208
connect 12 bit a/d chip via spi
* https://docs.micropython.org/en/latest/esp8266/library/machine.SPI.html
 * https://docs.micropython.org/en/latest/pyboard/library/pyb.SPI.html
 * https://github.com/doceme/py-spidev
 * https://www.modmypi.com/adafruit-industries/adafruit-raspberry-pi/accessories-515/chips/8-channel-10-bit-analog-to-digital-converter-(mcp3008)

* https://forum.micropython.org/viewtopic.php?t=1959
* https://github.com/avolkov/rpi-gardening/blob/master/tlug_presentation.md
* http://hertaville.com/interfacing-an-spi-adc-mcp3008-chip-to-the-raspberry-pi-using-c.html
* http://www.raspberrypi-spy.co.uk/2013/10/analogue-sensors-on-the-raspberry-pi-using-an-mcp3008/
 * https://github.com/Sillern/esp_mcp3008 -- C implementation
* https://github.com/adafruit/Adafruit_Python_MCP3008 -- python bit banging version
 * https://gist.github.com/ladyada/3151375 
 * https://github.com/micropython/micropython/blob/master/tests/pyb/spi.py

## battery powered
* http://blog.itechica.com/?p=92 -- current usage in deepsleep
 * no serial connection
 * delay(100)
 * pull down resistance on GPIO0 ?
* http://pic-microcontroller.com/running-nodemcu-on-a-battery/
* http://www.instructables.com/id/ESP8266-Li-Ion-Battery-rechargeable-battery-power-/

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

----
# arduino ide 
* https://github.com/esp8266/Arduino
* http://www.instructables.com/id/Programming-the-ESP8266-12E-using-Arduino-software/
* http://www.mikrocontroller-elektronik.de/nodemcu-esp8266-tutorial-wlan-board-arduino-ide/ (German)

## mcp3008
* https://arduino.stackexchange.com/questions/23507/arduino-esp8266-no-spi-data-coming-from-mcp3008#30152
* https://github.com/nodesign/MCP3008

## ads1015
* https://cmuphyscomp.github.io/60-223-f16/exercises/mobile/NodeMCU-I2C-ADC/index.html

----
# voltage regulator
In case the voltage regulator (AMS1117) doesn't fit your needs or was damaged
you may replace it with:
* L78L33
* LP2951 3.3V
* MCP1700

You could also try to run it directly from a 3.7V li-ion batterie without a voltage
regulator but with a small voltage dropping diode as described here:
* https://tinker.yeoman.com.au/2016/05/29/running-nodemcu-on-a-battery-esp8266-low-power-consumption-revisited/

----
## links
* https://github.com/nodemcu
* https://frightanic.com/iot/tools-ides-nodemcu/ -- comparison of tools
* https://forum.micropython.org/viewtopic.php?t=2240 -- EDIT-DEPLOY-RUN-TEST cycle
* https://github.com/squix78/esp8266-fritzing-parts -- nodemcu fritzig part
* https://github.com/lvidarte/esp8266/wiki -- good starting point as a workshop
