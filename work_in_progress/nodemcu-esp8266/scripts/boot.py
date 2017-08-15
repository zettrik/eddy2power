# This file is executed on every boot (including wake-boot from deepsleep
#import esp
#esp.osdebug(None)
#import webrepl
#import webrepl_setup
#webrepl.start()

import machine
if machine.reset_cause() == machine.DEEPSLEEP_RESET:
    print('bootet up from deep sleep')
else:
    print('power on or hard reset')

import gc
gc.collect()

