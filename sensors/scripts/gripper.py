import rospy
from std_srvs.srv import SetBool
from gpiozero import Servo
from time import sleep

servo = Servo(6)

def handle_toggle(gripper):
    req = gripper.data
    if req == True:
        servo.max()
    else:
        servo.min()
    return []

def listener():
    rospy.init_node('gripper')
    s = rospy.Service('gripper', SetBool, handle_toggle)
    rospy.spin()

if __name__ == '__main__':
    listener()


