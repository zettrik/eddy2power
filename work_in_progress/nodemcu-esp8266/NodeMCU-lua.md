# NodeMCU Firmware
usage of lua environment on NodeMCU

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


