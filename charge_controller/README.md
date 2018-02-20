# About
This is a proof of concept for a sophisticated charge controller based on
[NodeMCU](https://github.com/nodemcu). The idea derived from the wonderful
simple and rock solid [555Controller](http://mdpub.com/555Controller/).
A lot of precautions and hints were also taken from 
[Open Design Charge Regulator Project](https://www.re-innovation.co.uk/blog/2011/open-design-charge-regulator-project/).
The NodeMCU is a development board for an ESP8266 chip. It integrates a 3.3V regulator and USB communication.

## Enhancements to 555Controller:
 * measuring wind speed and blade rotation
 * data logging to sd-card
 * wireless control and monitoring of the internal state

## On The Downside:
 * way more power consumption for the controller itself
 * more complex setup


# Voltage Measurements
* https://www.re-innovation.co.uk/docs/accurate-voltage-measurment/
* https://www.re-innovation.co.uk/docs/dc-current-measurement/

# Power Consumption
* NodeMCU 95mA @ 3.3V with active wifi
* NodeMCU 38mA @ 3.3V without wifi
* SD-Card Adapter 20mA @ 3.3V
* MCP3208 0,5mA @ 3.3V
* RTC ?

# Power Supply
How to power the NodeMCU
## Voltage Regulator
In case the voltage regulator (AMS1117) doesn't fit your needs or was damaged
you may replace it with:
* LP2951 3.3V
* MCP1700
* L78L33 (max. 100mA which is probably not enough)
* https://www.tindie.com/products/nEXT_EVO1/30v-synchronous-step-down-dc-dc-converter/

## Battery Powered
* http://pic-microcontroller.com/running-nodemcu-on-a-battery/
* http://www.instructables.com/id/ESP8266-Li-Ion-Battery-rechargeable-battery-power-/
* http://blog.itechica.com/?p=92 -- power usage in deepsleep (in German)
 * no serial connection
 * delay(100)
 * pull down resistance on GPIO0 ?

## Li-ion Battery
You could also try to run it directly from a 3.7V li-ion battery without a voltage
regulator but with a small voltage dropping diode as described here. It worked
in my tests with a 18650 battery but might be dangerous in long term:
* https://tinker.yeoman.com.au/2016/05/29/running-nodemcu-on-a-battery-esp8266-low-power-consumption-revisited/

# Wifi
Monitoring of the charge controller is possible via wifi. Status
messages are regulary send to an mqttbroker.

## MQTT
* https://github.com/knolleary/pubsubclient -- arduino/esp8266 MQTT client
* https://pypi.python.org/pypi/paho-mqtt
* https://mosquitto.org/ -- lightweight MQTT broker (e.g. for wifi access points)
 * on openwrt/lede: opkg update; opkg install mosquitto-nossl
 * on debian: apt-get install mosquitto
 * listens by default on port 1883
* read mqtt channel on local mqtt broker:
 * aptitude install mosquitto-clients
 * mosquitto_sub -d -h broker -t "node/1/power"  # one channel
 * mosquitto_sub -d -h localhost -t "node/#" # all subchannels


# Circuit
![](nodemcu-wind-power-monitor.png)

* https://forum.mysensors.org/topic/6724/solved-nodemcu-esp8266-and-spi-with-nrf24l01-sd-card-module-sanity-errors/3
* http://www.electronicwings.com/nodemcu/nodemcu-i2c-with-arduino-ide

## Fritzing Parts
* https://github.com/squix78/esp8266-fritzing-parts -- NodeMCU
* https://github.com/adafruit/Fritzing-Library/tree/master/parts -- MCP3008
* ![MCP3208 8 channel 12-Bit ADC](MCP3208_8-channel_12-bit_ADC.fzpz) -- MCP3208

## PCB 
* http://bwir.de/pcb-platinen-mit-haushaltsmitteln-guenstig-selber-herstellen/ -- self made pcbs

# Links
* https://frightanic.com/iot/tools-ides-nodemcu/ -- comparison of development tools
* https://github.com/lvidarte/esp8266/wiki -- good starting point as a NodeMCU workshop

