#!/usr/bin/python3
import sys
from gpiozero import MCP3208

# Read data from the ADC 
def getData(sensor_nr):
    raw_value = 0
    read_repeat = 30
    # mittelwert
    for i in range(read_repeat):
        sensor = MCP3208(sensor_nr)
        raw_value += sensor.value
        #print(sensor.value)
        sensor.close()
    mcp = (raw_value * 4096 / read_repeat)
    return mcp

if len(sys.argv) == 2 and sys.argv[1] == "autoconf":
    print("yes")
elif len(sys.argv) == 2 and sys.argv[1] == "config":
    print('graph_title current @ mcp3208')
    print('graph_vlabel AD value')
    print('graph_category sensors')
    print('graph_args --logarithmic --base 1024')
    print('MCP4.label Sensor2 Strom' )
    print('MCP7.label Sensor1 Strom' )
else:
    print('MCP4.value %.2f' % (getData(4)-2065))
    print('MCP7.value %.2f' % (getData(7)-2077))
