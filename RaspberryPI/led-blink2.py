#import the GPIO and time package
import RPi.GPIO as GPIO
import time
GPIO.setmode(GPIO.BCM)
GPIO.setup(6, GPIO.OUT)
# loop through 50 times, on/off for 1 second
for i in range(2):
    GPIO.output(6,True)
    time.sleep(1)
    GPIO.output(6,False)
    time.sleep(1)
GPIO.output(6,False)
GPIO.cleanup()
