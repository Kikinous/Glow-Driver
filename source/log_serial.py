#!/usr/bin/env python

# Imports
import time
import serial
import os
import sys
import re

# Serial port parameters & initialization and open the port
#    possible timeout values:
#    1. None: wait forever, block call
#    2. 0: non-blocking mode, return immediately
#    3. x, x is bigger than 0, float allowed, timeout block call
ser = serial.Serial()
ser.port = "/dev/tty.HC-06-DevB"
ser.baudrate = 9600
ser.bytesize = serial.EIGHTBITS #number of bits per bytes
ser.parity = serial.PARITY_NONE #set parity check: no parity
ser.stopbits = serial.STOPBITS_ONE #number of stop bits
#ser.timeout = None          #block read
ser.timeout = 1            #non-block read
#ser.timeout = 2              #timeout block read
ser.xonxoff = False     #disable software flow control
ser.rtscts = False     #disable hardware (RTS/CTS) flow control
ser.dsrdtr = False       #disable hardware (DSR/DTR) flow control
ser.writeTimeout = 2     #timeout for write

response = "STDOUT:\n "

try: 
    ser.open()
except Exception, e:
    print "error open serial port: " + str(e)
    exit()

if ser.isOpen():
    try:
        ser.flushInput()  #flush input buffer, discarding all its contents
        ser.flushOutput() #flush output buffer, aborting current output 
                          #and discard all that is in buffer
        #write data
        #ser.write("AT+CSQ")
        #print("write data: AT+CSQ")
        time.sleep(0.5)   #give the serial port sometime to receive the data
        while True:

            response1 = ser.readline()
            response2 = ser.readline()
            matchObj = re.match( r'(Current)|(Temperature)', response1, re.I|re.X)
            if matchObj:
                os.system('cls' if os.name == 'nt' else 'clear')
                sys.stdout.write(response)
                sys.stdout.write("read data: " + response1)
                sys.stdout.write("read data: " + response2)
            else:
                matchObj = re.match( r'(Current)|(Temperature)', response1, re.I|re.X)
                if not matchObj:
                    sys.stdout.write(response1)
                    response = response + response1 
                matchObj = re.match( r'(Current)|(Temperature)', response2, re.I|re.X)
                if not matchObj:
                    sys.stdout.write(response2)
                    response = response + response2 
            #time.sleep(1)   #give the serial port sometime to receive the data
        ser.close()
    except Exception, e1:
        print "error communicating...: " + str(e1)
else:
    print "cannot open serial port "
