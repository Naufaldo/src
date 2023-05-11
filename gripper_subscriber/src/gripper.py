#!/usr/bin/env python3

import rospy
from std_msgs.msg import Bool
import RPi.GPIO as GPIO

# Set the GPIO pin for the servo motor
servo_pin = 6

# Initialize the GPIO pins
GPIO.setmode(GPIO.BCM)
GPIO.setup(servo_pin, GPIO.OUT)

# Set up PWM on the GPIO pin for the servo
pwm = GPIO.PWM(servo_pin, 50)
pwm.start(0)

# Create a callback function to handle incoming messages
def callback(msg):
    # If the message value is True, set the servo to 180 degrees
    if msg.data:
        pwm.ChangeDutyCycle(9)
    # If the message value is False, set the servo to 90 degrees
    else:
        pwm.ChangeDutyCycle(6.5)

# Initialize the node
rospy.init_node('gripper_subscriber')

# Create a subscriber with topic name '/gripper_control' and message type Bool
subscriber = rospy.Subscriber('servo_position', Bool, callback)

# Spin the node to receive messages
rospy.spin()

# Clean up the GPIO pins
GPIO.cleanup()
