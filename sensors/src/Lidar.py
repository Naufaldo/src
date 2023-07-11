#!/usr/bin/env python3

import rospy
from sensor_msgs.msg import LaserScan

def callback(msg):
    print('0:', msg.ranges[0])
    print('90:', msg.ranges[90])
    print('180:', msg.ranges[180])
    print('270:', msg.ranges[270])
    print('360:', msg.ranges[360])
    print('450:', msg.ranges[450])
    print('540:', msg.ranges[540])
    print('630:', msg.ranges[630])
    print('720:', msg.ranges[720])
    print('810:', msg.ranges[810])
    print('900:', msg.ranges[900])
    print('990:', msg.ranges[990])


rospy.init_node('data_lidar')
sub = rospy.Subscriber('/scan', LaserScan, callback)
rospy.spin()
