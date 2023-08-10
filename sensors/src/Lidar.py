#!/usr/bin/env python3

import rospy
from sensor_msgs.msg import LaserScan

def callback(msg):
    # Extract the distance at specific angles in centimeters
    S_0 = int(msg.ranges[0] * 100)
    S_90 = int(msg.ranges[90] * 100)
    S_180 = int(msg.ranges[180] * 100)
    S_270 = int(msg.ranges[270] * 100)

    # Print the distances at specified angles
    print('Distance at 0/360 degrees:', S_0, 'cm')
    print('Distance at 90 degrees:', S_90, 'cm')
    print('Distance at 180 degrees:', S_180, 'cm')
    print('Distance at 270 degrees:', S_270, 'cm')

rospy.init_node('data_lidar')
sub = rospy.Subscriber('/scan', LaserScan, callback)
rospy.spin()