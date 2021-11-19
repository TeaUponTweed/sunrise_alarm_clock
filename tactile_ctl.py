import RPi.GPIO as GPIO
import requests
import time

#server = '192.168.1.127:8080'
server = 'localhost:8080'
light_on = False

GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)
GPIO.setup(10, GPIO.IN, pull_up_down=GPIO.PUD_UP)

def button_callback(channel):
    global light_on
    if light_on:
        light_on = False
        print("Turning light off")
        requests.get(f'http://{server}/api/turn_off_light')
    else:
        light_on = True
        print("Turning light on")
        requests.get(f'http://{server}/api/turn_on_light')
    time.sleep(1)

GPIO.add_event_detect(10, GPIO.RISING, callback=button_callback)

message = input("Press 'enter' to quit\n\n")
GPIO.cleanup()

