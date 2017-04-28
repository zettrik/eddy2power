set title "Arduino sensor values"
set ylabel "A/D input"
set xlabel "date"
set xdata time
set timefmt x "%s"
#set format x "%g"
#set yrange [0:1030]
set grid ytics


set autoscale
#set logscale y

## print into png image
set output "eddy2power.png"
#set terminal png size 1440,900
set terminal png size 8000,900
plot \
    "eddy2power.csv" using 1:2 title 'A0' with lines, \
    "eddy2power.csv" using 1:3 title 'A1' with lines, \
    "eddy2power.csv" using 1:4 title 'A2' with lines, \
    "eddy2power.csv" using 1:5 title 'A3' with lines, \
    "eddy2power.csv" using 1:6 title 'A4' with lines, \
    "eddy2power.csv" using 1:7 title 'A5' with lines , \
    #"eddy2power.csv" using 1:8 title 'A6' with dots , \

## print on screen and wait ?? seconds
#set terminal windows
#plot ...
#pause 30
