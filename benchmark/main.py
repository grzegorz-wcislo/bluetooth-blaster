import bluetooth
import sys
import time
import math

port = 1
address = 'CC:50:E3:AB:CF:6E'

try: 
    sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
    sock.connect((address, port))
    print('Connected')
    sock.settimeout(100.0)

    start_time = time.monotonic()
    while True:
        data = sock.recv(1000)
        print(f'{time.monotonic() - start_time}: {len(data)}')
finally:
    print('Closing connection and exiting')
    sock.close()
