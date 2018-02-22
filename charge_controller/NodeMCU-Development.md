# NodeMCU with C++ and Arduino IDE
Advanced programming of the nodemcu can be done with the arduino ide
* https://github.com/esp8266/Arduino
* https://arduino-esp8266.readthedocs.io/en/latest/
* http://www.instructables.com/id/Programming-the-ESP8266-12E-using-Arduino-software/
* http://www.mikrocontroller-elektronik.de/nodemcu-esp8266-tutorial-wlan-board-arduino-ide/ (German)

## RTC
real time clock via i2c
* https://github.com/adafruit/RTClib
* http://datasheets.maximintegrated.com/en/ds/DS1307.pdf
* http://www.alldatasheet.com/view.jsp?Searchword=PCF8523

## Analog Digital Converter ADS1015
12 Bit 4 chanel ADC via I2C
* https://cmuphyscomp.github.io/60-223-f16/exercises/mobile/NodeMCU-I2C-ADC/index.html

## Analog Digital Converter MCP3208
12 Bit 8 Channel ADC via SPI
* https://arduino.stackexchange.com/questions/23507/arduino-esp8266-no-spi-data-coming-from-mcp3008#30152
* https://github.com/nodesign/MCP3008

### MCP3208 Dev Board
The ADS1015 is available as a breakout module. The MCP3208 is not. It seems not
so popular yet. So if you need a handy pcb you probably have to make it your own.
* http://forums.parallax.com/discussion/165675/pcbs-for-adc-chips-mcp3208-and-adc124s021
* order at oshpark, seeedstudio, ...

## other ADC
* http://www.esp8266-projects.com/2015/04/18-bit-adc-mcp3421-i2c-driver-esp8266.html/

## SD-Card Reader
read from and write to sd-cards
* http://www.esp8266.com/viewtopic.php?f=32&t=3558&start=12
* https://github.com/esp8266/Arduino/tree/master/libraries/SD

## Two Devices Via SPI
* https://forum.mysensors.org/topic/6724/solved-nodemcu-esp8266-and-spi-with-nrf24l01-sd-card-module-sanity-errors/3 
* https://forums.adafruit.com/viewtopic.php?f=25&p=276651
* https://forum.arduino.cc/index.php?topic=360718.0 


# NodeMCU with Micropython
The NodeMCU board runs fine with micropython. You will have an interactive python
shell on a microcontroller. :) No more pain in the ass(embler).

* http://micropython.org/download/
* http://docs.micropython.org/en/latest/esp8266/

## Install Esptool 
```
aptitude install python3-venv
python3 -m venv testproject
source testproject/bin/activate
pip install esptool
## save packagelist for later use with: pip install -r requirements.txt
pip freeze > requirements.txt

## afterwards
deactivate
```

## Flash Firmware With Esptool 
```
esptool.py --chip esp8266 --port /dev/ttyUSB0 erase_flash 
esptool.py --chip esp8266 --port /dev/ttyUSB0 --baud 460800 write_flash -fm dio 0 firmware.bin
```
* "-fm dio 0" is important for nodemcu boards!

## Micropython Benützen
### Connect To Serial Console
```
screen /dev/ttyUSB0 115200

## inside screen session type:
help()
```

Pressing ctrl-E will enter a special paste mode. This allows you to copy and
paste a chunk of text into the REPL.

### Wifi
Initially, the device configures itself as a WiFi access point (AP).

ESSID: MicroPython-xxxxxx (x’s are replaced with part of the MAC address).
Password: micropythoN (note the upper-case N).
IP address of the board: 192.168.4.1.
DHCP-server is activated.

### Development With Ampy
```
venv pip install adafruit-ampy
ampy -p /dev/ttyUSB0 get boot.py
```


## Example Pythonscript
```python
import machine
pin = machine.Pin(2, machine.Pin.OUT)
pin.on()
pin.off()
```

## Handle SD Cards
* build with micro sd card adapter
  * http://www.instructables.com/id/Cheap-DIY-SD-card-breadboard-socket/
* https://learn.adafruit.com/micropython-hardware-sd-cards/esp8266
* https://forum.micropython.org/viewtopic.php?f=16&t=3404
* wired connections:
```
| SD Card Pin | NodeMCU Pin (GPIO) |
| ---- | ---- |
| 1 CS   | D8 (GPIO15) |
| 2 DIN, MOSI  | D7 (GPIO13) |
| 3 GND  | GND |
| 4 VCC  | 3,3V |
| 5 CLK  | D5 (GPIO14) |
| 6 GND  | GND |
| 7 DOUT, MISO | D6 (GPIO12) |
```
* put sdcard.py in micropython environment and use like in sd.py
  * https://github.com/micropython/micropython/blob/master/drivers/sdcard/sdcard.py

## MCP3208
connect a 12 bit a/d chip via spi
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


# NodeMCU with Lua
Usage of lua environment on NodeMCU might be an easy starting point.

## Build Firmware For Your Needs
* https://nodemcu-build.com/index.php

## Firmware Flashing
### Install Esptool 
```
aptitude install python3-venv
python3 -m venv testproject
source testproject/bin/activate
pip install esptool

## save packagelist for later use with: pip install -r requirements.txt
pip freeze > requirements.txt

## afterwards...
deactivate
```

## Flash Firmware With Esptool
```
source testproject/bin/activate
esptool -cp /dev/ttyUSB0 -cd nodemcu -cb 9600 -cf firmware-integer.bin
deactivate
```

## Develop & Upload Lua Scripts
### Esplorer
* https://esp8266.ru/esplorer/ -- IDE and upload tool
  * https://github.com/4refr0nt/luatool -- cli upload

### NodeMCU Uploader
```
pip install nodemcu-uploader
nodemcu-uploader -B 9600 terminal
```
* firmware before mid 2016 used 9600 baudrate instead of 115200 now


