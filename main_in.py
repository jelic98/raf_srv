import serial
import time

arduino = serial.Serial("/dev/cu.usbmodem14101", baudrate=9600, timeout=3)
time.sleep(3)


while(True):
    response = arduino.readline().decode("ascii").replace("\r\n", "\n")
    print(repr(response))

    if(response[0] == "G"): # Graph
        coords = response.split(",")
        coords.pop(0)
        
        fgraph = open("graph.csv", "a+")
        fgraph.write(",".join(coords))
        fgraph.close()
    elif(response[0] == "C"): # Console
        fconsole = open("console.csv", "a+")
        fconsole.write(response)
        fconsole.close()
