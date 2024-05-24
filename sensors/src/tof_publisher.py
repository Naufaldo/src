#!/usr/bin/python

import time
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

# Create VL53L0X objects for each device
try:
    indices = [1, 3, 5, 7]
    for i in indices:
        sensor = VL53L0X.VL53L0X(tca9548a_num=i, tca9548a_addr=0x70)
        sensor.open()
        sensor.start_ranging(VL53L0X.Vl53l0xAccuracyMode.BETTER)
        sensors.append(sensor)
except Exception as e:
    rospy.logerr("Error initializing sensor {}: {}".format(i, str(e)))

# Get the timing from the first sensor if available
if sensors:
    timing = sensors[0].get_timing()
    if timing < 100000:
        timing = 100000
    rospy.loginfo("Timing %d ms" % (timing / 1000))

try:
    while not rospy.is_shutdown():
        sensor_distances = []
        has_error = False

        # Get distances from each sensor
        for i, sensor in enumerate(sensors):
            try:
                distance = sensor.get_distance()
                if distance > 0:
                    sensor_distances.append(distance)
                else:
                    rospy.logerr("Error: Invalid distance value from sensor %d" % i)
                    has_error = True
            except Exception as e:
                rospy.logerr("Error: Failed to get distance from sensor %d. Exception: %s" % (i, str(e)))
                has_error = True

        # Check if all sensors have published their distances
        if len(sensor_distances) == len(sensors):
            publish_distances(sensor_distances)
        else:
            rospy.logerr("Error: Failed to get distances from all sensors")
            publish_distances(sensor_distances)

        # Log the distances for front, back, left, and right sensors
        if len(sensor_distances) >= 4:
            rospy.loginfo("Front: %d mm, Back: %d mm, Left: %d mm, Right: %d mm" % (
                sensor_distances[0], sensor_distances[1], sensor_distances[2], sensor_distances[3]))
        else:
            rospy.logerr("Error: Insufficient sensor distances to log all directions")

        time.sleep(timing / 1000000.00)

except KeyboardInterrupt:
    # Stop ranging for all sensors when the program is interrupted
    for sensor in sensors:
        sensor.stop_ranging()

    rospy.loginfo("Program interrupted")

# Stop ranging for all sensors before exiting
for sensor in sensors:
    sensor.stop_ranging()
    sensor.close()
