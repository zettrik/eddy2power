#!/usr/bin/python
## this plugin starts a python script to return sensors data

import os, re, sys, urllib, subprocess

# check pluginname for sensors (anemometer_sensorname)
sensor = os.environ.get('code', sys.argv[0][(sys.argv[0].rfind('_') + 1):])

if sensor == None:
    sensor = 1

if len(sys.argv) == 2 and sys.argv[1] == "autoconf":
    print "yes"
elif len(sys.argv) == 2 and sys.argv[1] == "config":
    print 'graph_title Windspeed by sensor'
    print 'graph_vlabel windspeed in m/s'
    print 'graph_category sensors'
    print 'graph_args --base 1000 -l 0'
    print 'windspeed.label Sensor Stadthafen Rostock'
else:
    try:
        cmd = "python /usr/local/bin/anemometer_reader.py"
        p = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell = True, stderr=subprocess.STDOUT)
        speed, speed_err = p.communicate()
    except e:
        print(e)
    print 'windspeed.value %s' % str(speed)
