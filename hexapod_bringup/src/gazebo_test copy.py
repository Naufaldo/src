#!/usr/bin/env python

import rospy
from std_msgs.msg import Float64

def publish_joint_commands():
    # Initialize the ROS node
    rospy.init_node('hexapod_joint_publisher', anonymous=True)
    
    # List of joint controllers
    joint_controllers = [
        'coxa_joint_LR', 'femur_joint_LR', 'tibia_joint_LR', 'tarsus_joint_LR',
        'coxa_joint_RR', 'femur_joint_RR', 'tibia_joint_RR', 'tarsus_joint_RR',
        'coxa_joint_LM', 'femur_joint_LM', 'tibia_joint_LM', 'tarsus_joint_LM',
        'coxa_joint_RM', 'femur_joint_RM', 'tibia_joint_RM', 'tarsus_joint_RM',
        'coxa_joint_LF', 'femur_joint_LF', 'tibia_joint_LF', 'tarsus_joint_LF',
        'coxa_joint_RF', 'femur_joint_RF', 'tibia_joint_RF', 'tarsus_joint_RF'
    ]
    
    # Dictionary to store publishers
    publishers = {}

    # Create publishers for each joint controller
    for joint in joint_controllers:
        topic_name = '/Golem/{}_position_controller/command'.format(joint)
        publishers[joint] = rospy.Publisher(topic_name, Float64, queue_size=10)
    
    rate = rospy.Rate(10) # 10 Hz
    
    while not rospy.is_shutdown():
        # Define the command value (this example sets all joints to 0.5 radians)
        command_value = 0.5
        
        # Create a Float64 message
        command_msg = Float64()
        command_msg.data = command_value
        
        # Publish the command to each joint controller
        for joint, pub in publishers.items():
            pub.publish(command_msg)
            rospy.loginfo("Published command to {}: {}".format(joint, command_value))
        
        # Sleep to maintain the loop rate
        rate.sleep()

if __name__ == '__main__':
    try:
        publish_joint_commands()
    except rospy.ROSInterruptException:
        pass
