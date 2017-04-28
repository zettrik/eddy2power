#!/usr/bin/python
import sys

if len(sys.argv) == 2 and sys.argv[1] == "autoconf":
    print "yes"
elif len(sys.argv) == 2 and sys.argv[1] == "config":
    print 'graph_title sensor values @ arduino'
    print 'graph_vlabel A/D value'
    print 'graph_category sensors'
    print 'graph_args --base 1000 -l 0'
    print 'A0.label Sensor A0' 
    print 'A1.label Sensor A1' 
    print 'A2.label Sensor A2' 
    print 'A3.label Sensor A3' 
    print 'A4.label Sensor A4' 
    print 'A5.label Sensor A5' 
else:
    tmp_file = open("/tmp/eddy_last_values", 'r') 
    sensor_values = tmp_file.readline()
    tmp_file.close()
    A = sensor_values.split(", ")
    print 'A0.value %s' % str(A[1])
    print 'A1.value %s' % str(A[2])
    print 'A2.value %s' % str(A[3])
    print 'A3.value %s' % str(A[4])
    print 'A4.value %s' % str(A[5])
    print 'A5.value %s' % str(A[6])
