#!/usr/bin/env python3

import rospy
from std_msgs.msg import Float64MultiArray
from adafruit_extended_bus import ExtendedI2C as I2C
from adafruit_mpu6050 import MPU6050

def main():
    rospy.init_node('mpu6050_node')
    pub = rospy.Publisher('imu_data', Float64MultiArray, queue_size=10)
    rate = rospy.Rate(10)

    i2c = I2C(1)
    mpu = MPU6050(i2c)

    while not rospy.is_shutdown():
        accel_x, accel_y, accel_z = mpu.acceleration
        gyro_x, gyro_y, gyro_z = mpu.gyro

        imu_data = Float64MultiArray()
        imu_data.data = [accel_x, accel_y, accel_z, gyro_x, gyro_y, gyro_z]
        pub.publish(imu_data)

        rate.sleep()

if __name__ == '__main__':
    try:
        main()
    except rospy.ROSInterruptException:
        pass
