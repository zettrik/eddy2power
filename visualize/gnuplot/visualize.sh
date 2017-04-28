#!/bin/ash
#git pull
#cp ../../Messen/pi_python_scripts/eddy2power.sqlite.git .

#scp eddy.extern:~/windrad/Messen/pi_python_scripts/eddy2power.sqlite .
./sqlite2csv.py
#sed -i '/A/d' eddy2power.csv # remove lines containing a letter
lines=`cat eddy2power.csv | wc -l`
if [ ${lines} -gt "1" ]; then
    gnuplot draw_values.gnuplot
    eom eddy2power.png &
else
    echo "not enough data in csv file..."
    echo ${lines}
fi
exit 0
