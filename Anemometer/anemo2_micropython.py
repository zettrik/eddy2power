#!/usr/bin/env micropython
# this code monitors edges on a given gpio pin and counts the changes
import sys,utime

## init gpio
#rmmod gpio_button_hotplug #unload module to avoid concurrent gpio access
#echo 26 > /sys/class/gpio/export
#echo in

tick=0
tack=0
tock=0
edge_latest = 0
with open('/sys/class/gpio/gpio26/value', 'r') as file:
	edge_before = int(file.read(1))

time_before = utime.time()

for i in range(0,2000) :
	with open('/sys/class/gpio/gpio26/value', 'r') as file:
		edge_latest = int(file.read(1))
	if edge_before < edge_latest :
		tick += 1
	if edge_before > edge_latest :
		tack += 1
	else :
		tock += 1
	edge_before = edge_latest
	## sensor gives four edges per second at 2,4m/s windspeed
	## not expecting more than 200 raising edges/s (120m/s windspeed)
	utime.sleep(0.005)

time_after = utime.time()
duration = time_after - time_before
#print(duration)

#print("rising edges: " + str(tick))
#print("fallinge edges: " + str(tack))
#print("no changes: " + str(tock))

## 1/10 of the loop is overhead, which we substract here too
windspeed=tick*1.2/(duration-(duration/10))
print("windspeed.value: " + str(windspeed))

sys.exit()
