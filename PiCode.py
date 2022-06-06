import time
time.sleep(15)

#DHT imports
import Adafruit_DHT

# Laser imports
from PiicoDev_VL53L1X import PiicoDev_VL53L1X

# Webhook imports
import requests

#Communication with Arduino
import serial

DHT_SENSOR = Adafruit_DHT.DHT11
DHT_PIN = 4

distSensor = PiicoDev_VL53L1X()

#give the sensors 5 seconds to set up 
time.sleep(5)

#flags - to be sent to PARTICLE (for LCD display)
fanflag = " "
heatflag = " "

#airmessage legend - to be sent via serial to ARDUINO (temp control):
# - 0 = start
# - 10 = heat on
# - 20 = heat off
# - 30 = fan on
# - 40 = fan off
airmessage = "0"

# for robustness of measurement
tempsum = 0.0
distsum = 0.0
tempavg = 0.0
distavg = 0.0

#serial setup
ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
ser.flush()

def avgreadings():
    i=0 #to check the 'accurate reading' loop
    j=0 #loop will fail after 50 incorrect readings
    global tempsum 
    global distsum
    global tempavg
    global distavg 
    while (i < 5 and j < 100):
        humidity, temperature = Adafruit_DHT.read_retry(DHT_SENSOR, DHT_PIN)
        dist = distSensor.read() # read the distance in millimetres
        print("Temp={0:0.1f}C".format(temperature))
        if(temperature is not None and dist is not None):
            tempsum += temperature
            distsum += dist
            i += 1
            time.sleep(3)
        else:
            print("Reading skipped due to faulty hardware")
            j += 1
            time.sleep(3)
    tempavg = tempsum / 5
    distavg = distsum / 5

#main loop
while True:
    avgreadings()
    
    #overrides for debugging (test cases)
    #tempavg = 20
    #tempavg = 25
    #tempavg = 28
    #tempavg = 31
    #distavg = 233
    
    #this just prevents display issues during testing - won't cause issues during actual run
    if (distavg > 1000):
        distavg = 999
    
    tempStr = "{:2.1f}".format(tempavg)
    distStr = "{:3.0f}".format(distavg)
    
    if tempavg is not None and distavg is not None:
        #pi console log:
        print("Temp={0:0.1f}C Distance={1:0.1f}mm".format(tempavg, distavg))
        
        if (tempavg < 24):
            airmessage = "10" #for the serial (arduino)
            heatflag = "H" #for the LCD (particle)
        elif (tempavg >=24 and tempavg < 27):
            airmessage = "40"
            fanflag = " "
        elif (tempavg >=27 and tempavg < 30):
            airmessage = "20"
            heatflag = " "
        elif (tempavg >=30):
            airmessage = "30"
            fanflag = "F"
            
        #ifttt trigger is used by the particle for display:
        requests.post('https://maker.ifttt.com/trigger/tempsent/json/with/key/privatekey',
                      data={"value1":tempStr, "value2":distStr, "value3":fanflag,"value4":heatflag})
        
    else:
        print("Major sensor failure. No reading taken in 100 attempts. Check wiring. Loop will end.")
        requests.post('https://maker.ifttt.com/trigger/faults/json/with/key/privatekey',
                      data={"value1":"Sensor Fault - No Reading Taken"})
        break
    
    #overrides for debugging (test cases)
    #airmessage = "10" #heat on
    #airmessage = "20" #heat off
    #airmessage = "30" #fan on
    #airmessage = "40" #fan off
    
    ser.write(airmessage.encode('utf-8')) #serial code to be sent
       
    tempsum = 0.0
    distsum = 0.0
