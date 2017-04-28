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
    mcp = raw_value * 4096 / read_repeat
    return mcp

if len(sys.argv) == 2 and sys.argv[1] == "autoconf":
    print("yes")
elif len(sys.argv) == 2 and sys.argv[1] == "config":
    print('graph_title sensor values @ mcp3208')
    print('graph_vlabel AD value')
    print('graph_category sensors')
    print('graph_args -l 0')
    print('MCP0.label Sensor MCP0' )
    print('MCP1.label Sensor MCP1' )
    print('MCP2.label Sensor MCP2' )
    print('MCP3.label Sensor Umgebungslicht' )
    print('MCP4.label Sensor2 Strom' )
    print('MCP5.label Sensor2 Spannung' )
    print('MCP6.label Sensor1 Spannung' )
    print('MCP6.colour COLOUR0' )
    print('MCP7.label Sensor1 Strom' )
    print('MCP7.colour COLOUR1' )
else:
    print('MCP0.value %.2f' % (getData(0)))
    print('MCP1.value %.2f' % (getData(1)))
    print('MCP2.value %.2f' % (getData(2)))
    print('MCP3.value %.2f' % (getData(3)))
    print('MCP4.value %.2f' % (getData(4)))
    print('MCP5.value %.2f' % (getData(5)))
    print('MCP6.value %.2f' % (getData(6)))
    print('MCP7.value %.2f' % (getData(7)))
