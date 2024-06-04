#!/usr/bin/env python

import rospy
from std_msgs.msg import Float64
import cv2
import numpy as np

class HexapodControlGUI:
    def __init__(self):
        # Initialize the ROS node
        rospy.init_node('hexapod_joint_publisher', anonymous=True)
        
        # List of joint controllers
        self.joint_controllers = [
            'coxa_joint_LR', 'femur_joint_LR', 'tibia_joint_LR', 'tarsus_joint_LR',
            'coxa_joint_RR', 'femur_joint_RR', 'tibia_joint_RR', 'tarsus_joint_RR',
            'coxa_joint_LM', 'femur_joint_LM', 'tibia_joint_LM', 'tarsus_joint_LM',
            'coxa_joint_RM', 'femur_joint_RM', 'tibia_joint_RM', 'tarsus_joint_RM',
            'coxa_joint_LF', 'femur_joint_LF', 'tibia_joint_LF', 'tarsus_joint_LF',
            'coxa_joint_RF', 'femur_joint_RF', 'tibia_joint_RF', 'tarsus_joint_RF'
        ]
        
        # Dictionary to store publishers
        self.publishers = {}

        # Create publishers for each joint controller
        for joint in self.joint_controllers:
            topic_name = '/Golem/{}_position_controller/command'.format(joint)
            self.publishers[joint] = rospy.Publisher(topic_name, Float64, queue_size=10)

        # Create a window
        cv2.namedWindow('Hexapod Control')

        # Create trackbars (sliders) for each joint controller
        for joint in self.joint_controllers:
            cv2.createTrackbar(joint, 'Hexapod Control', 50, 100, self.nothing)

        # Initial update
        self.update_ros()

    def nothing(self, x):
        pass

    def update_ros(self):
        while not rospy.is_shutdown():
            for joint, pub in self.publishers.items():
                # Get the slider position
                slider_value = cv2.getTrackbarPos(joint, 'Hexapod Control')
                command_value = (slider_value - 50) / 50.0  # Scale to range -1.0 to 1.0
                
                # Create and publish the message
                command_msg = Float64()
                command_msg.data = command_value
                pub.publish(command_msg)
                rospy.loginfo("Published command to {}: {}".format(joint, command_value))
            
            # Display the window (necessary to keep the GUI active)
            cv2.waitKey(100)  # Update every 100ms

if __name__ == '__main__':
    try:
        gui = HexapodControlGUI()
    except rospy.ROSInterruptException:
        pass
    finally:
        cv2.destroyAllWindows()
