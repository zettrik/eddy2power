#!/bin/ash
## check which gpio is used
#cat /sys/kernel/debug/gpio

## init gpio
#rmmod gpio_button_hotplug #unload module to avoid concurrent gpio access
#echo 26 > /sys/class/gpio/export
#echo in > /sys/class/gpio/gpio26/direction

## run
while true; do

ticks=0
tocks=0
interval=10  #how many seconds to listen gpio changes
starttime=$(($(date +%s) + 2)) # add two seconds for clean start
endtime=$((starttime + interval))
## do some nonsense till starttime 
while [ $(date +%s) -lt $starttime ]; do
    ticks=0
done
echo 1 > /sys/class/leds/led0/brightness
#flanke=$(cat /sys/class/gpio/gpio26/value)
flanke=$(gpio -g read 4)
## increase starttime and count changes of gpio value
while [ $endtime -gt $starttime ]; do
    starttime=$(date +%s)
    #flanke2=$(cat /sys/class/gpio/gpio26/value)
    flanke2=$(gpio -g read 4)
    if [ "$flanke" -ne "$flanke2" ]; then
        ticks=$(( ticks + 1 ))
        flanke=$flanke2
    else
        # no gpio change, just count the loop
        tocks=$(( tocks +1 ))
        flanke=$flanke
    fi
done
loops=$(($ticks+$tocks))
echo "TICKS:" $ticks
echo "LOOPS:" $loops
echo 0 > /sys/class/leds/led0/brightness
#sleep 280
done
exit 0
