#/bin/ash
##unload module to avoid concurrent gpio access
#rmmod gpio_button_hotplug 
## check which gpio is used
cat /sys/kernel/debug/gpio

# GPIO 22: tests...
#echo 22 > /sys/class/gpio/export
#echo out > /sys/class/gpio/gpio22/direction
## switch light on
echo "1" > /sys/class/gpio/gpio22/value
cat /sys/kernel/debug/gpio
sleep 10  
## switch light off
echo "0" > /sys/class/gpio/gpio22/value
cat /sys/kernel/debug/gpio

exit 0

