import RPi.GPIO as GPIO
from time import sleep
import os
os.chdir('/home/pi/Documents/project')
import lcd
import smbus
from datetime import datetime

GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)

buzzer=22 #15
button=27 #13
led=17 #11

E1=41
F1=44

song = [
  E1, F1
]

beat = [
  8,8
]

GPIO.setup(buzzer, GPIO.OUT)
GPIO.setup(button,GPIO.IN)
GPIO.setup(led,GPIO.OUT)
Buzz = GPIO.PWM(buzzer, 440)

i2c_ch = 1

# TMP102 address on the I2C bus
i2c_address = 0x4d

# Read temperature in Celsius
def read_temp():

    #Read temperature celsius
    temp = bus.read_byte_data(i2c_address,0)
    return temp

# Initialize I2C (SMBus)
bus = smbus.SMBus(i2c_ch)

while True:
    GPIO.setmode(GPIO.BCM)
    #setup pin buzzer as output
    GPIO.setup(buzzer, GPIO.OUT)
    #setup pin button as input
    GPIO.setup(button, GPIO.IN)
    #setup pin led as output
    GPIO.setup(led, GPIO.OUT)
    #initialize Display LCD
    lcd.lcd_init()
    print("init")
    #get time
    now = datetime.now()
    current_time = now.strftime("%H:%M")
    #read temperature
    temperature = read_temp()
    #print blank space in first line of LCD
    lcd.lcd_byte(lcd.LCD_LINE_1, lcd.LCD_CMD)
    lcd.lcd_string(" ", 2)
    #print time and temperature in second line of LCD
    lcd.lcd_byte(lcd.LCD_LINE_2, lcd.LCD_CMD)
    time_temp= str(current_time)+" "+str(int(temperature))+" C"
    lcd.lcd_string(time_temp, 2)
    #if button is pressed
    if GPIO.input(button) == True:
        print("Beep")
        #turn on led
        GPIO.output(led, GPIO.HIGH)
        #print STOP in first line of LCD
        lcd.lcd_byte(lcd.LCD_LINE_1, lcd.LCD_CMD)
        lcd.lcd_string("STOP",2)
        #initialize buzzer
        Buzz.start(50)
        #change frequency of buzzer
        for j in range(1, len(song)):
            Buzz.ChangeFrequency(song[j])
            sleep(beat[j]*0.13)
        sleep(3)
    #stop buzzer
    Buzz.stop()
    #turn off led
    GPIO.output(led, GPIO.LOW)
    print("No Beep")
    sleep(0.5)
    #clean up GPIO from LCD
    lcd.GPIO.cleanup()
    print("clean done")
    
        
