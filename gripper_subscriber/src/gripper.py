#!/usr/bin/env python3

import rospy
from std_msgs.msg import Int32
import RPi.GPIO as GPIO

# Set the GPIO pins for the LEDs
led_pin_1 = 17
led_pin_2 = 18
# Set the GPIO pin for the button
button_pin = 26

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

# Spin the node to receive messages
rospy.spin()

# Clean up the GPIO pins
GPIO.cleanup()
