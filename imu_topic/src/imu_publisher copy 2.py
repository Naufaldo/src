#!/usr/bin/env python3

import rospy
from sensor_msgs.msg import Imu
from std_msgs.msg import Float64
from adafruit_mpu6050 import MPU6050
from time import sleep
import board


class MPU6050Node:
    def __init__(self):
        rospy.init_node('mpu6050_node')
        self.pub_imu = rospy.Publisher('/imu/data', Imu, queue_size=10)
        self.pub_temp = rospy.Publisher('/imu/temperature', Float64, queue_size=10)
        self.rate = rospy.Rate(100)  # Publish at 100 Hz
        i2c = board.I2C()
        self.mpu = MPU6050(i2c)

    def read_imu_data(self):
        accel_x, accel_y, accel_z = self.mpu.acceleration
        gyro_x, gyro_y, gyro_z = self.mpu.gyro
        temp = self.mpu.temperature

        return accel_x, accel_y, accel_z, gyro_x, gyro_y, gyro_z, temp

    def run(self):
        while not rospy.is_shutdown():
            imu_data = Imu()
            accel_x, accel_y, accel_z, gyro_x, gyro_y, gyro_z, temp = self.read_imu_data()

            # Publish IMU data
            imu_data.linear_acceleration.x = accel_x
            imu_data.linear_acceleration.y = accel_y
            imu_data.linear_acceleration.z = accel_z
            imu_data.angular_velocity.x = gyro_x
            imu_data.angular_velocity.y = gyro_y
            imu_data.angular_velocity.z = gyro_z
            imu_data.header.stamp = rospy.Time.now()
            imu_data.header.frame_id = 'imu_link'
            self.pub_imu.publish(imu_data)

            # Publish temperature data
            temp_msg = Float64()
            temp_msg.data = temp
            self.pub_temp.publish(temp_msg)

            self.rate.sleep()


if __name__ == '__main__':
    try:
        mpu6050_node = MPU6050Node()
        mpu6050_node.run()
    except rospy.ROSInterruptException:
        pass
