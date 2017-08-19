print("started main")
import os
print("machine: %s" % os.uname())
print("files on rootfs: %s" % os.listdir())

import led
led.start()

#import wifi

