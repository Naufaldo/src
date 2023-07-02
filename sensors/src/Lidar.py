#!/usr/bin/env python3

import rospy
from sensor_msgs.msg import LaserScan

def callback(msg):
    print('0:', msg.ranges[0])
    print('180:', msg.ranges[180])
    print('360:', msg.ranges[360])
    print('540:', msg.ranges[540])

rospy.init_node('data_lidar')
sub = rospy.Subscriber('/scan', LaserScan, callback)
rospy.spin()
