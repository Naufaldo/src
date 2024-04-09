#!/usr/bin/env python

import rospy
from sensor_msgs.msg import Imu
from geometry_msgs.msg import PoseStamped
from tf.transformations import euler_from_quaternion

class ImuToEulerConverter:
    def __init__(self):
        rospy.init_node('imu_to_euler_converter_node')
        self.imu_sub = rospy.Subscriber('/imu/data', Imu, self.imu_callback)
        self.euler_pub = rospy.Publisher('/euler_topic', PoseStamped, queue_size=10)

    def imu_callback(self, msg):
        # Convert quaternion to Euler angles
        quat = (msg.orientation.x, msg.orientation.y, msg.orientation.z, msg.orientation.w)
        roll, pitch, yaw = euler_from_quaternion(quat)

        # Create Euler angle message
        euler_msg = PoseStamped()
        euler_msg.header = msg.header
        euler_msg.pose.position.x = roll
        euler_msg.pose.position.y = pitch
        euler_msg.pose.position.z = yaw

        # Publish Euler angle message
        self.euler_pub.publish(euler_msg)

if __name__ == '__main__':
    try:
        converter = ImuToEulerConverter()
        rospy.spin()
    except rospy.ROSInterruptException:
        pass