#!/usr/bin/env python

import rospy
from std_msgs.msg import Float32MultiArray
from sensor_msgs.msg import Imu

# Import ADXL345 library
import Adafruit_ADXL345

def imu_publisher():
    # Initialize the ROS node
    rospy.init_node('imu_publisher', anonymous=True)

    # Create a publisher for the IMU topic
    imu_pub = rospy.Publisher('imu_data', Imu, queue_size=10)

    # Initialize the ADXL345 sensor
    adxl345 = Adafruit_ADXL345.ADXL345()

    # Set the range of the sensor (optional)
    adxl345.set_range(Adafruit_ADXL345.ADXL345_RANGE_16_G)

    # Set the data rate (optional)
    adxl345.set_data_rate(Adafruit_ADXL345.ADXL345_DATARATE_100_HZ)

    # Create the IMU message
    imu_msg = Imu()

    rate = rospy.Rate(10)  # Publish at 10 Hz

    while not rospy.is_shutdown():
        # Read the acceleration values from the ADXL345 sensor
        accel = adxl345.read()

        # Set the IMU message fields
        imu_msg.linear_acceleration.x = accel[0]
        imu_msg.linear_acceleration.y = accel[1]
        imu_msg.linear_acceleration.z = accel[2]

        # Publish the IMU message
        imu_pub.publish(imu_msg)

        rate.sleep()

if __name__ == '__main__':
    try:
        imu_publisher()
    except rospy.ROSInterruptException:
        pass
