import serial
import time

arduino = serial.Serial("/dev/cu.usbserial-1410", baudrate=9600, timeout=3)
time.sleep(3)
fout = open("graph.csv", "w")

while(True):
    request = str(input("Request: ")).encode()
    arduino.write(request)

    response = arduino.readline().decode("ascii").replace("\n", "")
    print(response)

    if(response[0] == "G"):
        fout.write(response + "\r\n")

fout.close()
