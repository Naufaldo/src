#!/usr/bin/env python3

import rospy
from std_msgs.msg import Int32
from std_msgs.msg import Bool
import RPi.GPIO as GPIO
import subprocess
from hexapod_msgs.msg import MergedPingArray

# Set the GPIO pins for the LEDs
led_pin_1 = 17
led_pin_2 = 18
# Set the GPIO pin for the button
button_pin = 26

ping = [0, 0, 0, 0, 0]
thresholds = [10, 15, 12, 8, 20]

# Initialize the GPIO pins
GPIO.setmode(GPIO.BCM)
GPIO.setup(led_pin_1, GPIO.OUT)
GPIO.setup(led_pin_2, GPIO.OUT)
GPIO.setup(button_pin, GPIO.IN, pull_up_down=GPIO.PUD_UP)


# Create a callback function to handle button press
def button_callback(channel):
    # Publish a message to the 'button_pressed' topic
    pub.publish(Bool(True))

# Initialize the node
rospy.init_node('gripper_subscriber')

# def merged_ping_callback(data):
#     global ping
#     for i in range(5):
#         ping[i] = data.merged_ping_array[i]
    
    # Check if any value in the ping array is below its corresponding threshold
    # for i in range(5):
    #     if ping[i] < thresholds[i]:
    #         subprocess.call("Shell_scripts/Gerakan1.sh", shell=True)


# Set up the publisher for the button press
pub = rospy.Publisher('button_pressed', Bool, queue_size=10)

# Add an event detection for button press
GPIO.add_event_detect(button_pin, GPIO.FALLING, callback=button_callback, bouncetime=200)

# Function to handle LED control messages
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

# Create a subscriber for the LED control
led_subscriber = rospy.Subscriber('led_control', Int32, led_callback)
# rospy.Subscriber('merged_ping_topic', MergedPingArray, merged_ping_callback)

# Spin the node to receive messages
rospy.spin()

# Clean up the GPIO pins
GPIO.cleanup()
