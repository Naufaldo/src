#!/usr/bin/env bash

# Launch the robot
source /opt/ros/noetic/setup.bash 
source /home/pi/Ancabots/devel/setup.bash 

# Running Node Index
NAVIGATION_MODE=1

# Start the navigation launch file in the background
roslaunch hexapod_bringup navigation_$NAVIGATION_MODE.launch &