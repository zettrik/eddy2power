import machine
## freq can be 80 or 160MHz
print(machine.freq())

def overclock():
    machine.freq(160000000)
    print(machine.freq())

def underclock():
    machine.freq(80000000)
    print(machine.freq())
