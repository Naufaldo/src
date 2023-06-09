#!/usr/bin/env python3

import rospy
from std_msgs.msg import Header
from sensor_msgs.msg import Imu

# Import the MPU6050 library
from mpu6050 import mpu6050

def publish_imu_data():
    rospy.init_node('imu_node', anonymous=True)
    imu_pub = rospy.Publisher('imu/data', Imu, queue_size=10)
    rate = rospy.Rate(10)  # Publishing rate (10 Hz)

    # Create an instance of the MPU6050 sensor
    mpu = mpu6050(0x68)  # Replace with the correct I2C address

    while not rospy.is_shutdown():
        # Read MPU6050 data
        accel_data = mpu.get_accel_data()
        gyro_data = mpu.get_gyro_data()

        # Create an Imu message and populate it with the sensor data
        imu_msg = Imu()
        imu_msg.header = Header()
        imu_msg.header.stamp = rospy.Time.now()
        imu_msg.angular_velocity.x = gyro_data['x']
        imu_msg.angular_velocity.y = gyro_data['y']
        imu_msg.angular_velocity.z = gyro_data['z']
        imu_msg.linear_acceleration.x = accel_data['x']
        imu_msg.linear_acceleration.y = accel_data['y']
        imu_msg.linear_acceleration.z = accel_data['z']

        # Publish the Imu message
        imu_pub.publish(imu_msg)
        rate.sleep()

if __name__ == '__main__':
    try:
        publish_imu_data()
    except rospy.ROSInterruptException:
        pass
