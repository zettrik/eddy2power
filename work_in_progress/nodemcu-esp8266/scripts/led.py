import machine
import time
import math

def switch():
    print("led test switching")
    pin = machine.Pin(2, machine.Pin.OUT)
    for i in range(30):
        pin.on()
        time.sleep_ms(50)
        pin.off()
        time.sleep_ms(50)

def pulse(l, t):
    for i in range(1023):
        ## duty must be between 0-1023
        #l.duty(int(math.sin(i / 10 * 3.1415) * 500 + 500))
        l.duty(i)
        time.sleep_ms(t)
    for i in range(1023):
        l.duty(1023 - i)
        time.sleep_ms(t)

def start():
    p2 = machine.Pin(2, machine.Pin.OUT)
    ## frequency must be between 1-1000 and the same for all pins
    pwm2 = machine.PWM(p2, freq=1000)
    print("led test pulsing")
    for i in range(2):
        pulse(pwm2, 1)

