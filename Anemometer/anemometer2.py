#!/usr/bin/env python
from time import sleep
import os, time
import RPi.GPIO as GPIO

SENSOR_PIN = 4

GPIO.setmode(GPIO.BCM)
GPIO.setup(SENSOR_PIN, GPIO.IN, pull_up_down=GPIO.PUD_DOWN) #pin that the anemometer is connected to
#GPIO.setup(25, GPIO.OUT) # pin that the LED is connected to

timer = time.time() # initialise the timer
fast_time = -1 # TODO: future use
wind_str = 0
print "time, windspeed in m/s, time for revolution"
def wind_ping(channel):
    global timer
    global fast_time
    global wind_str
    cur_time = (time.time() - timer) # time for half a revolution
    wind_speed = 0.667 / cur_time # reworked from the datasheet
    if wind_speed < 100: # add bounce detection here
        if cur_time < fast_time:
            fast_time = cur_time
        wind_str = "%10.4f" % wind_speed # convert the float number to a string
        wind_speed = wind_speed * 2.236936 # apply the multiplier to calculate miles per hour
        windmph_str = "%10.4f" % wind_speed
        cur_str = "%10.4f" % cur_time
        # will print the measurements to screen for each pulse detected
        #print "Wind Speed: " + wind_str + " m/s, " + windmph_str + " mph, time: " + cur_str
        timer = time.time() #reset the timer for the next revolution
        now = "%10.4f" % timer
        print now + ", " + wind_str + ", " + cur_str
        #GPIO.output(25, not GPIO.input(25)) # alternate the LED state


GPIO.add_event_detect(SENSOR_PIN, GPIO.FALLING, bouncetime=30) #threaded event, to detect the voltage falling on anemometer (pin 4)
GPIO.add_event_callback(SENSOR_PIN, wind_ping)   # tell the event to call procedure above

try:
    while True: # loop to keep the program alive
        #print wind_str
        sleep(1)
except KeyboardInterrupt:
    GPIO.cleanup() # reset the GPIO pins when you press ctrl+c

