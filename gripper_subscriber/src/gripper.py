#!/usr/bin/env python3

import rospy
from std_msgs.msg import UInt16
import RPi.GPIO as GPIO

SERVO_PIN = 6

def servo_callback(msg):
    # Set the duty cycle for the servo based on the received message
    duty_cycle = msg.data

    # Limit the duty cycle to the valid range

    # Write the duty cycle to the PWM pin
    pwm = GPIO.PWM(SERVO_PIN, 50)
    pwm.start(2.5)
    pwm.ChangeDutyCycle(duty_cycle)

def main():
    # Initialize the ROS node
    rospy.init_node('gripper_subscriber')

    # Set up GPIO pin 6 as a PWM output
    GPIO.setwarnings(False)
    GPIO.setmode(GPIO.BCM)
    GPIO.setup(SERVO_PIN, GPIO.OUT)
    

    # Create a subscriber for the servo position
    servo_sub = rospy.Subscriber('servo_position', UInt16, servo_callback)

    # Spin and wait for callbacks
    rospy.spin()

if __name__ == '__main__':
    main()
