#!/usr/bin/python3
import sys

def getData(temp_filedescriptor):
    temperature = 0
    read_repeat = 10
    # mittelwert
    for i in range(read_repeat):
        tempfile = open(temp_filedescriptor)
        temptext = tempfile.read()
        tempfile.close()
        tempdata = temptext.split("\n")[1].split(" ")[9]
        temperature += float(tempdata[2:])
    return temperature / (read_repeat * 1000)

if len(sys.argv) == 2 and sys.argv[1] == "autoconf":
    print("yes")
elif len(sys.argv) == 2 and sys.argv[1] == "config":
    print('graph_title temperatures @ ds18b20')
    print('graph_vlabel temperature in C')
    print('graph_category sensors')
    print('graph_args -l 0')
    print('T1.label T1' )
    print('T2.label T2' )
else:
    print('T1.value %.2f' % (getData("/sys/bus/w1/devices/28-0000074abb83/w1_slave")))
    print('T2.value %.2f' % (getData("/sys/bus/w1/devices/28-0000074ae5cf/w1_slave")))
 
