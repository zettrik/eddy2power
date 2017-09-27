#!/bin/bash
nodemcu="/dev/ttyUSB0"

killall screen
source testmcu/bin/activate
files=$@
for file in ${files}; do
    echo "put on node: ${file}"
    ampy -p ${nodemcu} put ${file}
done
deactivate

screen ${nodemcu} 115200

exit 0

