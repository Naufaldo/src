#!/usr/bin/env python3

import rospy
from std_msgs.msg import Bool, Float32MultiArray
import RPi.GPIO as GPIO
import time

# Assign GPIO Pins for each sensor
GPIO_SIG = [17, 18, 19, 20, 21]  # Modify these pins based on your wiring

# Set the GPIO pin for the servo motor
servo_pin = 6

# Initialize the GPIO pins
GPIO.setmode(GPIO.BCM)
GPIO.setup(servo_pin, GPIO.OUT)

# Set up PWM on the GPIO pin for the servo
pwm = GPIO.PWM(servo_pin, 50)
pwm.start(0)

# Create a callback function to handle incoming servo position messages
def servo_callback(msg):
    # If the message value is True, set the servo to 180 degrees
    if msg.data:
        pwm.ChangeDutyCycle(8)
    # If the message value is False, set the servo to 90 degrees
    else:
        pwm.ChangeDutyCycle(4)

# Create a callback function to handle incoming sensor data messages
def sensor_callback(msg):
    # Access the sensor data array
    sensor_data = []

    for i, pin in enumerate(GPIO_SIG):
        # Measure distance for each sensor
        GPIO.setup(pin, GPIO.OUT)
        GPIO.output(pin, 0)

        time.sleep(0.000002)

        # Send trigger signal
        GPIO.output(pin, 1)

        time.sleep(0.000005)

        GPIO.output(pin, 0)

        GPIO.setup(pin, GPIO.IN)

        while GPIO.input(pin) == 0:
            starttime = time.time()

        while GPIO.input(pin) == 1:
            endtime = time.time()

        duration = endtime - starttime
        # Distance is defined as time/2 (there and back) * speed of sound 34000 cm/s
        distance = (duration * 34000) / 2

        sensor_data.append(distance)

    # Process and use the sensor data as needed
    for i, distance in enumerate(sensor_data):
        print("Sensor", i+1, "distance:", distance)

# Initialize the node
rospy.init_node('sensor_and_gripper')

# Create a subscriber for the servo position control with topic name '/servo_position' and message type Bool
servo_subscriber = rospy.Subscriber('servo_position', Bool, servo_callback)

# Create a subscriber for the sensor data with topic name '/sensor_data' and message type Float32MultiArray
sensor_subscriber = rospy.Subscriber('sensor_data', Float32MultiArray, sensor_callback)

# Spin the node to receive messages
rospy.spin()

# Clean up the GPIO pins
GPIO.cleanup()
