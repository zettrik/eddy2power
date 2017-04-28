#!/usr/bin/python3
from time import sleep
from gpiozero import MCP3208

# Read data from the ADC 
def getData(sensor_number):
    sensor = MCP3208(sensor_number)
    rawValue = sensor.value
    sensor.close()
    return rawValue*4096

while True:
    print('%.0f | %.0f | %0.f | %0.f | %0.f | %0.f | %0.f | %0.f ' \
        % (getData(0), getData(1), getData(2), getData(3), \
            getData(4), getData(5), getData(6), getData(7)))
    sleep(0.1)

