#!/usr/bin/env python

import rospy
from sensor_msgs.msg import Imu
from std_msgs.msg import Float64
import math
import time
from adafruit_extended_bus import ExtendedI2C as I2C
from adafruit_mpu6050 import MPU6050


class IMUNode:
    def __init__(self):
          rospy.init_node('mpu6050_node')
        self.pub_imu = rospy.Publisher('/imu/data', Imu, queue_size=10)
        self.pub_temp = rospy.Publisher('/imu/temperature', Float64, queue_size=10)
        self.rate = rospy.Rate(100)  # Publish at 100 Hz
        i2c = board.I2C()
        self.mpu = MPU6050(i2c)
        self.dt = 0.1  # Time step in seconds
        self.alpha = 0.98  # Complementary filter coefficient
        self.roll = 0.0
        self.pitch = 0.0
        self.yaw = 0.0

    def read_imu_data(self):
        accel_x, accel_y, accel_z = self.imu.acceleration
        gyro_x, gyro_y, gyro_z = self.imu.gyro
        temp = self.imu.temperature

        accel_x /= 9.80665  # Convert acceleration to m/s^2
        accel_y /= 9.80665
        accel_z /= 9.80665

        gyro_x *= math.pi / 180.0  # Convert gyro values to rad/s
        gyro_y *= math.pi / 180.0
        gyro_z *= math.pi / 180.0

        return accel_x, accel_y, accel_z, gyro_x, gyro_y, gyro_z, temp

    def update_orientation(self, accel_x, accel_y, accel_z, gyro_x, gyro_y, gyro_z):
        # Convert accelerometer values to roll and pitch angles
        roll_acc = math.atan2(accel_y, accel_z)
        pitch_acc = math.atan2(-accel_x, math.sqrt(accel_y**2 + accel_z**2))

        # Calculate roll and pitch angles using a complementary filter
        self.roll = self.alpha * (self.roll + gyro_x * self.dt) + (1 - self.alpha) * roll_acc
        self.pitch = self.alpha * (self.pitch + gyro_y * self.dt) + (1 - self.alpha) * pitch_acc

        # Calculate yaw angle using gyroscope data
        self.yaw += gyro_z * self.dt

        # Normalize yaw angle to the range [-pi, pi]
        if self.yaw > math.pi:
            self.yaw -= 2 * math.pi
        elif self.yaw < -math.pi:
            self.yaw += 2 * math.pi

    def run(self):
        while not rospy.is_shutdown():
            imu_data = Imu()
            accel_x, accel_y, accel_z, gyro_x, gyro_y, gyro_z, temp = self.read_imu_data()

            self.update_orientation(accel_x, accel_y, accel_z, gyro_x, gyro_y, gyro_z)

            # Publish IMU data
            imu_data.orientation.x = self.roll
            imu_data.orientation.y = self.pitch
            imu_data.orientation.z = self.yaw
            imu_data.orientation.w = 0.0  # No orientation w component
            imu_data.linear_acceleration.x = accel_x
            imu_data.linear_acceleration.y = accel_y
            imu_data.linear_acceleration.z = accel_z
            imu_data.angular_velocity.x = gyro_x
            imu_data.angular_velocity.y = gyro_y
            imu_data.angular_velocity.z = gyro_z
            imu_data.header.stamp = rospy.Time.now()
            imu_data.header.frame_id = 'imu_link'

            # Set covariance values
            linear_accel_cov = [0.04, 0.0, 0.0, 0.0, 0.04, 0.0, 0.0, 0.0, 0.04]
            imu_data.linear_acceleration_covariance = linear_accel_cov

            angular_vel_cov = [0.02, 0.0, 0.0, 0.0, 0.02, 0.0, 0.0, 0.0, 0.02]
            imu_data.angular_velocity_covariance = angular_vel_cov

            orientation_cov = [-1, 0.0, 0.0, 0.0, -1, 0.0, 0.0, 0.0, -1]
            imu_data.orientation_covariance = orientation_cov

            self.pub_imu.publish(imu_data)

            # Publish temperature data
            temp_msg = Float64()
            temp_msg.data = temp
            self.pub_temp.publish(temp_msg)

            self.rate.sleep()


if __name__ == '__main__':
    try:
        imu_node = IMUNode()
        imu_node.run()
    except rospy.ROSInterruptException:
        pass
