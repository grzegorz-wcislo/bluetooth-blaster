import bluetooth
import sys
import signal

port = 1
address = 'CC:50:E3:AB:CF:6E'

running = True
def signal_handler(sig, frame):
    global running
    running = False

signal.signal(signal.SIGINT, signal_handler)

try: 
    sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
    sock.connect((address, port))
    print('Connected')
    sock.settimeout(100.0)

    while running:
        data = sock.recv(1000)
        print(data)
finally:
    print('Closing connection and exiting')
    sock.close()
