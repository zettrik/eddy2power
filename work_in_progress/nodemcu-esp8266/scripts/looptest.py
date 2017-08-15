#! /usr/bin/micropython
import time
import machine 

#time.sleep(1)           # sleep for 1 second
#time.sleep_ms(500)      # sleep for 500 milliseconds
#time.sleep_us(10)       # sleep for 10 microseconds

#p2 = machine.Pin(2, machine.Pin.OUT)    # create output pin on GPIO0
#p2.on()                 # set pin to "on" (high) level
#p2.off()                # set pin to "off" (low) level
#p2.value(1)             # set pin to on/high

def start(pin_number, loops):
    #pin_number = 2
    p = machine.Pin(pin_number, machine.Pin.IN)     # create input pin on GPIO2
    print("pin %i start value: %i" % (pin_number, p.value()))       # get value, 0 or 1
    print("loops running: %i" % loops)

    tick=0
    tack=0
    tock=0
    edge_before = p.value()
    edge_latest = 0
    start = time.ticks_ms() # get millisecond counter

    for i in range(1, loops) :
        #time.sleep_ms(1) 
        edge_latest = p.value()
        #print(edge_latest)
        if edge_before < edge_latest :
            tick += 1
        if edge_before > edge_latest :
            tack += 1
        else :
            tock += 1
        edge_before = edge_latest

    delta = time.ticks_diff(time.ticks_ms(), start) # compute time difference
    print("rising edges: " + str(tick))
    print("fallinge edges: " + str(tack))
    print("no changes: " + str(tock))
    print("time delta (ms): %i" % delta)

    sum = 0
    adc = machine.ADC(0)
    start = time.ticks_ms() # get millisecond counter
    for i in range(1, loops) :
        #time.sleep_ms(1) 
        analog= adc.read()
        sum += analog
    delta = time.ticks_diff(time.ticks_ms(), start) # compute time difference
    print("adc sum: %i" % sum)
    print("time delta (ms): %i" % delta)

