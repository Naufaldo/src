import rospy
from std_msgs.msg import Bool
import RPi.GPIO as GPIO
import time

servoPIN = 6
GPIO.setmode(GPIO.BCM)
GPIO.setup(servoPIN, GPIO.OUT)


def handle_toggle(gripper):
    p = GPIO.PWM(servoPIN, 50) # GPIO 17 for PWM with 50Hz
    p.start(2.5) # Initialization
    req = gripper.data
    if req == True:
        p.ChangeDutyCycle(13)
    else:
        p.ChangeDutyCycle(7.5)
    return []

def listener():
    rospy.init_node('gripper')
    s = rospy.Service('gripper', Bool, handle_toggle)
    rospy.spin()

if __name__ == '__main__':
    listener()


