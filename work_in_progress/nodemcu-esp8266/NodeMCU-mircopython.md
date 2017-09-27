# Micropython Firmware
NodeMCU Board runs fine with micropython. You will have an interactive python
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
```
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

