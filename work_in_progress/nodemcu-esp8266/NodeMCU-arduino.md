# Arduino IDE for NodeMCU
programming the nodemcu also can be done with the arduino ide
* https://github.com/esp8266/Arduino
* https://arduino-esp8266.readthedocs.io/en/latest/
* http://www.instructables.com/id/Programming-the-ESP8266-12E-using-Arduino-software/
* http://www.mikrocontroller-elektronik.de/nodemcu-esp8266-tutorial-wlan-board-arduino-ide/ (German)


# Extending IOs

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

