#!/usr/bin/env python3

import rospy
from std_msgs.msg import Float32MultiArray
from sensor_msgs.msg import Imu

# Import the required libraries
import time
from Adafruit_MPU6050 import MPU6050

def imu_publisher():
    # Initialize the ROS node
    rospy.init_node('imu_publisher', anonymous=True)

    # Create a publisher for the IMU topic
    imu_pub = rospy.Publisher('imu_data', Imu, queue_size=10)

    # Initialize the MPU6050 sensor
    mpu6050 = MPU6050()

    # Create the IMU message
    imu_msg = Imu()

    rate = rospy.Rate(10)  # Publish at 10 Hz

    while not rospy.is_shutdown():
        # Read the accelerometer data from the MPU6050 sensor
        accel_data = mpu6050.get_accel_data()

        # Read the gyroscope data from the MPU6050 sensor
        gyro_data = mpu6050.get_gyro_data()

        # Set the IMU message fields for linear acceleration
        imu_msg.linear_acceleration.x = accel_data['x']
        imu_msg.linear_acceleration.y = accel_data['y']
        imu_msg.linear_acceleration.z = accel_data['z']

        # Set the IMU message fields for angular velocity
        imu_msg.angular_velocity.x = gyro_data['x']
        imu_msg.angular_velocity.y = gyro_data['y']
        imu_msg.angular_velocity.z = gyro_data['z']

        # Publish the IMU message
        imu_pub.publish(imu_msg)

        rate.sleep()

if __name__ == '__main__':
    try:
        imu_publisher()
    except rospy.ROSInterruptException:
        pass
