import network

ap = network.WLAN(network.AP_IF)
ap.config(essid="eddy2power", password="12345678")
ap.active(True)
