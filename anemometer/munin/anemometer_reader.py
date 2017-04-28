#!/usr/bin/env python
'''munin plugin for reading anemometer pulses on gpio pins on raspberry pi'''

import time, sys
import RPi.GPIO as GPIO

class anemo():
    def __init__(self):
        ''' initialize the pin for the anemometer sensor '''
        self.SENSOR_PIN = 27
        self.count = 0
        # tell the GPIO module that we want to use the chip's pin numbering scheme
        GPIO.setmode(GPIO.BCM)
        # setup pin as an input with pullup
        GPIO.setup(self.SENSOR_PIN, GPIO.IN, pull_up_down=GPIO.PUD_UP) 
        # threaded event, to detect voltage falling on anemometer
        # bouncetime is in ms - edges within this time will be ignored
        GPIO.add_event_detect(self.SENSOR_PIN, GPIO.FALLING, bouncetime=30)
        self.starttime = time.time()
        # deal with events by calling a function
        GPIO.add_event_callback(self.SENSOR_PIN, self.inputEventHandler)

    def inputEventHandler(self, pin):
        ''' count the edges and calculate windspeed... 
            with "triggerflanke" you decide how much falling edges to count 
            until we start the speed calculation
            small values will result in short reaction time und precise values
            high values will take longer and give a good average over the time
            very high values may need a longer sleep value in the main method
            espacially at low wind speeds
        '''
        self.count += 1
        triggerflanke = 8
        if self.count == triggerflanke:
            # the sensor ticks twice per rotation (2 falling edges)
            # so with triggerflanke=20 we have the time for ten rotations
            currenttime = (time.time() - self.starttime)
            # calculating windspeed
            #windspeed = triggerflanke / (currenttime * 1.3)
            ## todo check calculation e.g. factor 8
            windspeed = (2.4 * 3.6) / (currenttime / (triggerflanke / 8))
            # exit printing the windspeed
            sys.exit(windspeed)

    def cleanup(self):
        ''' reset the pins to default state '''
        GPIO.cleanup()

if __name__ == "__main__":
    anemo = anemo()
    # let the programm run for at least x seconds. to measure low windspeed
    # this should be four times the triggerflanke
    time.sleep(32)
    sys.exit("0") # no data - no wind - return zero
