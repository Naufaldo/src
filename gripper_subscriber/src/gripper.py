#!/usr/bin/env python3
#asli
import rospy
from std_msgs.msg import Int32, Bool
from std_msgs.msg import Int32MultiArray
import RPi.GPIO as GPIO
import subprocess

# Set the GPIO pins for the LEDs
led_pin_1 = 5
led_pin_2 = 6
# Set the GPIO pin for the button
button_pin = 26

ping = [0, 0, 0, 0, 0]
thresholds = [10, 15, 12, 8, 20]

# Initialize the GPIO pins
GPIO.setmode(GPIO.BCM)
GPIO.setup(led_pin_1, GPIO.OUT)
GPIO.setup(led_pin_2, GPIO.OUT)
GPIO.setup(button_pin, GPIO.IN, pull_up_down=GPIO.PUD_UP)

def merged_ping_callback(data):
    global ping
    ping = data.data

# Create a callback function to handle button press
def button_callback(channel):
    # Publish a message to the 'button_pressed' topic
    if ping[3] > 300 and ping[0] < 200:
        #Uncomment kalo Arena Merah
        #subprocess.call("/home/pi/Ancabots/src/Shell_script/Gerakan1.sh", shell=True)
        #Uncomment kalo Arena Biru
        subprocess.call("/home/pi/Ancabots/src/Shell_script/Gerakan3.sh", shell=True)
    elif ping[0] > 300 and ping[3] < 200:
        #Uncomment kalo Arena Merah
        #subprocess.call("/home/pi/Ancabots/src/Shell_script/Gerakan2.sh", shell=True)
        #Uncomment kalo Arena Biru
        subprocess.call("/home/pi/Ancabots/src/Shell_script/Gerakan4.sh", shell=True)
    else:
        pub.publish(Bool(True))

def led_callback(msg):
    led_value = msg.data
    if led_value == 1:
        GPIO.output(led_pin_1, GPIO.HIGH)
        GPIO.output(led_pin_2, GPIO.LOW)
    elif led_value == 2:
        GPIO.output(led_pin_1, GPIO.HIGH)
        GPIO.output(led_pin_2, GPIO.HIGH)
    else:
        GPIO.output(led_pin_1, GPIO.LOW)
        GPIO.output(led_pin_2, GPIO.LOW)


# Initialize the node
rospy.init_node('button_and_led_subscriber')

# Set up the publisher for the button press
pub = rospy.Publisher('button_pressed', Bool, queue_size=10)

# Add an event detection for button press
GPIO.add_event_detect(button_pin, GPIO.FALLING, callback=button_callback, bouncetime=200)

# Create a subscriber for the merged ping data
sub = rospy.Subscriber('tof_distances', Int32MultiArray, merged_ping_callback)

# Create a subscriber for the LED control
led_subscriber = rospy.Subscriber('led_control', Int32, led_callback)

# Spin the node to receive messages
rospy.spin()

# Clean up the GPIO pins
GPIO.cleanup()
