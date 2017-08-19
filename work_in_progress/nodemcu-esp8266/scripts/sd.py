import machine, os, sdcard
"""
get a fresh version of sdcard.py from:
https://github.com/micropython/micropython/blob/master/drivers/sdcard/sdcard.py
"""

sd = sdcard.SDCard(machine.SPI(1), machine.Pin(15))
vfs = os.VfsFat(sd)
os.mount(vfs, "/sd")
print("files on SD-Card: %s" % os.listdir("/sd"))

time.sleep(1)
print("performing write test")
f = open("/sd/test.txt", "w")
f.write("hello world")
f.close()
print("files on SD-Card: %s" % os.listdir("/sd"))

time.sleep(1)
print("performing read test")
f = open("/sd/text.txt", "r")
print("file content %s" % f.read())
f.close()
