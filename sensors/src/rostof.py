#!/usr/bin/env python3

import time
import sys
sys.path.append('/home/pi/VL53L0X_rasp_python/python')
import VL53L0X
import rospy
from std_msgs.msg import Int32MultiArray

def publish_distances(distances):
    # Publish the distances as Int32MultiArray
    msg = Int32MultiArray(data=distances)
    pub.publish(msg)

rospy.init_node('tof_publisher', anonymous=True)
pub = rospy.Publisher('tof_distances', Int32MultiArray, queue_size=10)

# Create a list to store the VL53L0X objects for each sensor
sensors = []

# Create VL53L0X objects for devices on TCA9548A buses 0 to 7
for i in range(8):
    sensor = VL53L0X.VL53L0X(TCA9548A_Num=i, TCA9548A_Addr=0x70)
    sensor.start_ranging(VL53L0X.VL53L0X_BETTER_ACCURACY_MODE)
    sensors.append(sensor)

# Get the timing from the first sensor
timing = sensors[0].get_timing()
if timing < 100000:
    timing = 100000
print("Timing %d ms" % (timing / 1000))

try:
    while not rospy.is_shutdown():
        distances = []

        # Get distances from each sensor
        for i, sensor in enumerate(sensors):
            try:
                distance = sensor.get_distance()
                if distance > 0:
                    distances.append(distance)
                else:
                    print("Error: Invalid distance value from sensor %d" % i)
            except Exception as e:
                print("Error: Failed to get distance from sensor %d. Exception: %s" % (i, str(e)))

        # Check if all 8 sensors have published their distances
        if len(distances) == 8:
            publish_distances(distances)
        else:
            print("Error: Failed to get distances from all sensors")
            publish_distances(distances)

        time.sleep(timing / 1000000.00)
except KeyboardInterrupt:
    # Stop ranging for all sensors when program is interrupted
    for sensor in sensors:
        sensor.stop_ranging()

    rospy.loginfo("Program interrupted")

# Stop ranging for all sensors before exiting
for sensor in sensors:
    sensor.stop_ranging()
