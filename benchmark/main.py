import bluetooth
import sys
from timer import Timer

port = 1
address = 'CC:50:E3:AB:CF:6E'

try: 
    sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
    sock.connect((address, port))
    print('Connected', file=sys.stderr)
    sock.settimeout(100.0)

    t = Timer()
    while True:
        data = sock.recv(1000)
        t.add_received_data(data)
finally:
    print('Closing connection and exiting', file=sys.stderr)
    sock.close()
