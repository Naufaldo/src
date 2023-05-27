#!/usr/bin/env python3

import time
import rospy
from std_msgs.msg import Int32
from hexapod_msgs.msg import MergedPingArray
import board
import busio
import adafruit_vl53l0x

ping_data = [None, None, None, None, None, None]
pub = None

def ping_1_callback(data):
    global ping_data
    ping_data[0] = data.data

def ping_2_callback(data):
    global ping_data
    ping_data[1] = data.data

def ping_3_callback(data):
    global ping_data
    ping_data[2] = data.data

def ping_4_callback(data):
    global ping_data
    ping_data[3] = data.data

def ping_5_callback(data):
    global ping_data
    ping_data[4] = data.data

def publish_merged_ping_data(distance):
    global ping_data, pub
    ping_data[5] = distance
    merged_ping_array_msg = MergedPingArray()
    merged_ping_array_msg.merged_ping_array = ping_data

    pub.publish(merged_ping_array_msg)

def merged_topics():
    global pub
    rospy.init_node('ping_fusion_node')

    rospy.Subscriber('ping_1_topic', Int32, ping_1_callback)
    rospy.Subscriber('ping_2_topic', Int32, ping_2_callback)
    rospy.Subscriber('ping_3_topic', Int32, ping_3_callback)
    rospy.Subscriber('ping_4_topic', Int32, ping_4_callback)
    rospy.Subscriber('ping_5_topic', Int32, ping_5_callback)

    pub = rospy.Publisher('merged_ping_topic', MergedPingArray, queue_size=10)

    rate = rospy.Rate(10)  # Publish rate of 10 Hz

    # Initialize I2C bus and sensor
    i2c = busio.I2C(board.SCL, board.SDA)
    vl53 = adafruit_vl53l0x.VL53L0X(i2c)
    vl53.measurement_timing_budget = 200000  # Set measurement timing budget

    while not rospy.is_shutdown():
        # Process the VL53L0X sensor data here
        distance = vl53.range
        if distance > 0:
            publish_merged_ping_data(distance)

        time.sleep(0.1)

        publish_merged_ping_data(0)  # Placeholder for VL53L0X data when not available
        rate.sleep()

if __name__ == '__main__':
    merged_topics()
