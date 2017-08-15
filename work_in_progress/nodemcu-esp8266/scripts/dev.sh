#!/bin/bash
nodemcu="/dev/ttyUSB0"

killall screen
for file in `ls |grep \.py`; do
    echo "put on node: ${file}"
    ampy -p ${nodemcu} put ${file}
done
#file="led.py looptest.py"
ampy -p ${nodemcu} put ${file}
screen ${nodemcu} 115200
