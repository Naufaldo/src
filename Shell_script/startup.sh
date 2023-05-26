#!/usr/bin/env bash

# Launch the robot
source /opt/ros/noetic/setup.bash 
source /home/pi/Ancabots/devel/setup.bash 

echo "Launching application, please wait!"
roslaunch hexapod_bringup hexapod_test.launch