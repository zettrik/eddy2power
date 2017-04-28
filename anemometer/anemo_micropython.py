#! /usr/bin/micropython
with open('/sys/class/gpio/gpio26/value', 'r') as file:
	tick = file.read(1)
print("initial value: _" + tick + "_")
tick=0
tock=0
tack=0
flanke2 = 0
for i in range(0,60000) :
	#fp = open('/sys/class/gpio/gpio26/value', 'r')
    	#flanke = int(fp.read(1))
	#fp.close()
	with open('/sys/class/gpio/gpio26/value', 'r') as file:
		flanke = int(file.read(1))
	if flanke2 < flanke :
		tick += 1
	else :
		tack += 1
	tock += 1
	flanke2 = flanke
print("rising edges: " + str(tick))
print("fallinge edges: " + str(tack))
print("no changes: " + str(tock))
print(tick/2)


