#!/usr/bin/env python3

import rospy
from std_msgs.msg import Bool, Float32MultiArray
import RPi.GPIO as GPIO
import time

# Assign GPIO Pins for each sensor
GPIO_SIG = [17, 18, 19, 20, 21]  # Modify these pins based on your wiring
#kanan depan , kanan belakang, tengah belakang, kiri belakang, kiri depan

# Set the GPIO pin for the servo motor
servo_pin = 6

# Initialize the GPIO pins
GPIO.setmode(GPIO.BCM)


# Create a publisher for the sensor data
sensor_publisher = rospy.Publisher('sensor_data', Float32MultiArray, queue_size=10)

# Create a callback function to handle incoming servo position messages
def servo_callback(msg):
    GPIO.setup(servo_pin, GPIO.OUT)
    GPIO.setwarnings(False)
    # Set up PWM on the GPIO pin for the servo
    pwm = GPIO.PWM(servo_pin, 50)
    pwm.start(0)
    # If the message value is True, set the servo to 180 degrees
    if msg.data:
        pwm.ChangeDutyCycle(8)
    # If the message value is False, set the servo to 90 degrees
    else:
        pwm.ChangeDutyCycle(4)

# Function to measure distance for each sensor
def measure_distance(pin):
    GPIO.setup(pin, GPIO.OUT)
    GPIO.setwarnings(False)
    GPIO.output(pin, 0)

    time.sleep(0.000002)

    # Send trigger signal
    GPIO.output(pin, 1)

    time.sleep(0.000005)

    GPIO.output(pin, 0)

    GPIO.setup(pin, GPIO.IN)
    GPIO.setwarnings(False)
    while GPIO.input(pin) == 0:
        starttime = time.time()

    while GPIO.input(pin) == 1:
        endtime = time.time()

    duration = endtime - starttime
    # Distance is defined as time/2 (there and back) * speed of sound 34000 cm/s
    distance = (duration * 34000) / 2

    return distance

# Function to publish sensor data
def publish_sensor_data():
    sensor_data = []
    for pin in GPIO_SIG:
        distance = measure_distance(pin)
        sensor_data.append(distance)

    # Create a Float32MultiArray message
    sensor_msg = Float32MultiArray(data=sensor_data)

    # Publish the sensor data
    sensor_publisher.publish(sensor_msg)

# Initialize the node
rospy.init_node('sensor_and_gripper')

# Create a subscriber for the servo position control with topic name '/servo_position' and message type Bool
servo_subscriber = rospy.Subscriber('servo_position', Bool, servo_callback)

# Set the publishing rate for the sensor data
rate = rospy.Rate(1)  # 1 Hz

# Publish the sensor data at the specified rate
while not rospy.is_shutdown():
    publish_sensor_data()
    rate.sleep()

# Clean up the GPIO pins
GPIO.cleanup()
