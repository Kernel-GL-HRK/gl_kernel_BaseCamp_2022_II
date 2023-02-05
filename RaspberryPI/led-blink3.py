#import the GPIO and time package
import RPi.GPIO as GPIO
import time

GPIO.setmode(GPIO.BCM)

GPIO.setup(6, GPIO.OUT)
GPIO.setup(13, GPIO.OUT)
GPIO.setup(19, GPIO.OUT)

# loop through 50 times, on/off for 1 second
for i in range(50):
    GPIO.output(6,True)
    time.sleep(1)
    GPIO.output(6,False)
    time.sleep(1)
    GPIO.output(13,True)
    time.sleep(1)
    GPIO.output(13,False)
    time.sleep(1)
    GPIO.output(19,True)
    time.sleep(1)
    GPIO.output(19,False)
    time.sleep(1)

GPIO.output(6,False)
GPIO.output(19,False)
GPIO.output(13,False)

GPIO.cleanup()
