#!/usr/bin/env python

import rospy
from std_msgs.msg import Float32

import RPi.GPIO as GPIO
import time

def callback(data):
    angle = data.data
    duty = angle / 18 + 2
    GPIO.output(6, True)
    pwm.ChangeDutyCycle(duty)
    time.sleep(0.5)
    GPIO.output(6, False)
    pwm.ChangeDutyCycle(0)

def gripper_subscriber():
    rospy.init_node('gripper_subscriber', anonymous=True)

    GPIO.setmode(GPIO.BCM)
    GPIO.setup(6, GPIO.OUT)
    pwm = GPIO.PWM(6, 50)
    pwm.start(0)

    rospy.Subscriber('gripper_angle', Float32, callback)

    rospy.spin()

    pwm.stop()
    GPIO.cleanup()

if __name__ == '__main__':
    try:
        gripper_subscriber()
    except rospy.ROSInterruptException:
        pass
