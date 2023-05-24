#!/usr/bin/env python3

import rospy
from std_msgs.msg import Int32

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

def merged_ping_data(events):
    global ping_1_data, ping_2_data, ping_3_data, ping_4_data, ping_5_data
    print(f"Ping 1: {ping_1_data} | Ping 2: {ping_2_data} | Ping 3: {ping_3_data} | Ping 4: {ping_4_data} | Ping 5: {ping_5_data}")

def merged_topics():
    rospy.init_node('ping_fusion_subscriber')
    
    rospy.Subscriber('ping_1_topic', Int32, ping_1_callback)
    rospy.Subscriber('ping_2_topic', Int32, ping_2_callback)
    rospy.Subscriber('ping_3_topic', Int32, ping_3_callback)
    rospy.Subscriber('ping_4_topic', Int32, ping_4_callback)
    rospy.Subscriber('ping_5_topic', Int32, ping_5_callback)
    
    rospy.Timer(rospy.Duration(0.1), merged_ping_data)

    rospy.spin()

if __name__ == '__main__':
    merged_topics()
