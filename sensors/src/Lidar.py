#!/usr/bin/env python3

import rospy
from sensor_msgs.msg import LaserScan

def callback(msg):
    print('0:', msg.ranges[0])
    print('90:', msg.ranges[90])
    print('180:', msg.ranges[180])
    print('270:', msg.ranges[270])


rospy.init_node('data_lidar')
sub = rospy.Subscriber('/scan', LaserScan, callback)
rospy.spin()
