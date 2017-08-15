import machine
# configure RTC.ALARM0 to be able to wake the device
rtc = machine.RTC()
rtc.irq(trigger=rtc.ALARM0, wake=machine.DEEPSLEEP)
# set RTC.ALARM0 to fire after some milliseconds (waking the device)
rtc.alarm(rtc.ALARM0, 3000)
print("going to sleep")
machine.deepsleep()

