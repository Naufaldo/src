#!/usr/bin/env python3

import rospy
from std_msgs.msg import Int32
from hexapod_msgs.msg import MergedPingArray

ping_1_data = None
ping_2_data = None
ping_3_data = None
ping_4_data = None
ping_5_data = None

def ping_1_callback(data):
    global ping_1_data
    ping_1_data = data.data

def ping_2_callback(data):
    global ping_2_data
    ping_2_data = data.data

def ping_3_callback(data):
    global ping_3_data
    ping_3_data = data.data

def ping_4_callback(data):
    global ping_4_data
    ping_4_data = data.data

def ping_5_callback(data):
    global ping_5_data
    ping_5_data = data.data

def publish_merged_ping_data():
    global ping_1_data, ping_2_data, ping_3_data, ping_4_data, ping_5_data

    merged_ping_array_msg = MergedPingArray()
    merged_ping_array_msg.merged_ping_array = [ping_1_data, ping_2_data, ping_3_data, ping_4_data, ping_5_data]

    pub.publish(merged_ping_array_msg)

def merged_topics():
    rospy.init_node('ping_fusion_node')

    rospy.Subscriber('ping_1_topic', Int32, ping_1_callback)
    rospy.Subscriber('ping_2_topic', Int32, ping_2_callback)
    rospy.Subscriber('ping_3_topic', Int32, ping_3_callback)
    rospy.Subscriber('ping_4_topic', Int32, ping_4_callback)
    rospy.Subscriber('ping_5_topic', Int32, ping_5_callback)

    pub = rospy.Publisher('merged_ping_topic', MergedPingArray, queue_size=10)

    rate = rospy.Rate(10)  # Publish rate of 10 Hz

    while not rospy.is_shutdown():
        publish_merged_ping_data()
        rate.sleep()

if __name__ == '__main__':
    merged_topics()
