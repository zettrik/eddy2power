#/bin/ash
##unload module to avoid concurrent gpio access
#rmmod gpio_button_hotplug 
## check which gpio is used
cat /sys/kernel/debug/gpio

## GPIO 23: reset arduino with setting pin to low
#echo 23 > /sys/class/gpio/export
#echo out > /sys/class/gpio/gpio23/direction
echo "0" > /sys/class/gpio/gpio23/value
sleep 1  # few millisecs would be enough
echo "1" > /sys/class/gpio/gpio23/value

exit 0

