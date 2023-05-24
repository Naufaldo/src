#!/usr/bin/env python

import rospy
from std_msgs.msg import Float32MultiArray

def ping_callback(data):
    ping_data = data.data
    # Process the sonar data here
    print("Received sonar data:", ping_data)

rospy.init_node('ping_subscriber')
rospy.Subscriber('ping_data', Float32MultiArray, ping_callback)

rospy.spin()
