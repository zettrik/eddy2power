## nodemcu has one 10bit ad conversion pin with max. 1V!
## 0 = 0V and 1024 = 1V
import machine
adc = machine.ADC(0)
print("analog in: %i" % adc.read())
