import serial
import time

arduino = serial.Serial("/dev/cu.usbserial-1410", baudrate=9600, timeout=3)
time.sleep(3)

fgraph = open("graph.csv", "w+")
fconsole = open("console.csv", "w+")

while(True):
    request = str(input("Request: ")).encode()
    arduino.write(request)

    response = arduino.readline().decode("ascii").replace("\r\n", "")
    print(response)

    if(response[0] == "G"): # Graph
        coords = response.split(",")
        coords.pop(0)
        fgraph.write(",".join(coords))
    elif(response[0] == "C"): # Console
        fconsole.write(response)
    elif(response[0] == "E"): # Exit
        fgraph.close()
        fconsole.close()
