import RPi.GPIO as GPIO
from time import sleep
import os
os.chdir('/home/pi/Documents/project')
import lcd

GPIO.setwarnings(False)
#GPIO.setmode(GPIO.BOARD)
GPIO.setmode(GPIO.BCM)

buzzer=22 #15
button=27 #13
led=17 #11
GPIO.setup(buzzer, GPIO.OUT)
GPIO.setup(button,GPIO.IN)
GPIO.setup(led,GPIO.OUT)

#GPIO.setwarnings(True)

while True:
    GPIO.setmode(GPIO.BCM)
    GPIO.setup(buzzer, GPIO.OUT)
    GPIO.setup(button, GPIO.IN)
    GPIO.setup(led, GPIO.OUT)
    lcd.lcd_init()
    #lcd.lcd_byte(lcd.LCD_LINE_1, lcd.LCD_CMD)
    if GPIO.input(button) == True:
        print("Beep")
        for i in range(4):
            GPIO.output(buzzer, GPIO.HIGH)
            sleep(0.5)
            GPIO.output(buzzer, GPIO.LOW)
            GPIO.output(led, GPIO.HIGH)
            lcd.lcd_byte(lcd.LCD_LINE_1, lcd.LCD_CMD)
            lcd.lcd_string("STOP",2)
        sleep(3)
    else:
        GPIO.output(buzzer, GPIO.LOW)
        GPIO.output(led, GPIO.LOW)
        #lcd.lcd_string("Continue",2)
        print("No Beep")
        sleep(0.5)
    lcd.GPIO.cleanup()
        
