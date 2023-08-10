#!/usr/bin/env python3

import rospy
from sensor_msgs.msg import LaserScan

def callback(msg):

    print(' Depan 585:', msg.ranges[585]*100)
    print(' Kanan 270:', msg.ranges[270]*100)
    print(' Belakang 0:', msg.ranges[0]*100)
    print(' Kiri 855:', msg.ranges[855]*100)


rospy.init_node('data_lidar')
sub = rospy.Subscriber('/scan', LaserScan, callback)
rospy.spin()
