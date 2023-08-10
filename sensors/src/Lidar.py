#!/usr/bin/env python3

import rospy
from sensor_msgs.msg import LaserScan

def callback(msg):
    print('0:', msg.ranges[0]*100)
    print('90:', msg.ranges[90]*100)
    print('180:', msg.ranges[180]*100)
    print('270:', msg.ranges[270]*100)
    print('360:', msg.ranges[360]*100)
    print('450:', msg.ranges[450]*100)
    print('540:', msg.ranges[540]*100)
    print('630:', msg.ranges[630]*100)
    print('720:', msg.ranges[720]*100)
    print('810:', msg.ranges[810]*100)
    print('900:', msg.ranges[900]*100)
    print('990:', msg.ranges[990]*100)


rospy.init_node('data_lidar')
sub = rospy.Subscriber('/scan', LaserScan, callback)
rospy.spin()
