#!/usr/bin/env python3

import rospy
from std_msgs.msg import Bool
import RPi.GPIO as GPIO
import subprocess

button_pin = 26

# Initialize the GPIO pins
GPIO.setmode(GPIO.BCM)
GPIO.setup(button_pin, GPIO.IN, pull_up_down=GPIO.PUD_UP)

# Create a callback function to handle button press
def button_callback(channel):
    # Execute the shell scripts
    subprocess.call("/home/pi/Ancabots/src/Shell_script/Gerakan4.sh", shell=True)
    # Publish a message to the 'button_pressed' topic
    pub.publish(Bool(True))

# Initialize the ROS node
rospy.init_node('button_and_led_subscriber')

# Set up the publisher for the button press
pub = rospy.Publisher('button_pressed', Bool, queue_size=10)

# Add an event detection for button press
GPIO.add_event_detect(button_pin, GPIO.FALLING, callback=button_callback, bouncetime=200)

# Spin the node to receive messages
rospy.spin()

# Clean up the GPIO pins
GPIO.cleanup()
