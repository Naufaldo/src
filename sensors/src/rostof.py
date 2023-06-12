#!/usr/bin/env python3

import time
import sys
sys.path.append('/home/pi/VL53L0X_rasp_python/python')
import VL53L0X
import RPi.GPIO as GPIO
import rospy
from std_msgs.msg import Int32MultiArray

# GPIO for Sensor 1 shutdown pin
sensor1_shutdown = 4
# GPIO for Sensor 2 shutdown pin
sensor2_shutdown = 13
# GPIO for Sensor 3 shutdown pin
sensor3_shutdown = 19
# GPIO for Sensor 4 shutdown pin
sensor4_shutdown = 22
# GPIO for Sensor 5 shutdown pin
sensor5_shutdown = 12
# GPIO for Sensor 6 shutdown pin
sensor6_shutdown = 16
# GPIO for Sensor 7 shutdown pin
sensor7_shutdown = 20
# GPIO for Sensor 8 shutdown pin
sensor8_shutdown = 21

GPIO.setwarnings(False)

# Setup GPIO for shutdown pins on each VL53L0X
GPIO.setmode(GPIO.BCM)
GPIO.setup(sensor1_shutdown, GPIO.OUT)
GPIO.setup(sensor2_shutdown, GPIO.OUT)
GPIO.setup(sensor3_shutdown, GPIO.OUT)
GPIO.setup(sensor4_shutdown, GPIO.OUT)
GPIO.setup(sensor5_shutdown, GPIO.OUT)
GPIO.setup(sensor6_shutdown, GPIO.OUT)
GPIO.setup(sensor7_shutdown, GPIO.OUT)
GPIO.setup(sensor8_shutdown, GPIO.OUT)

# Set all shutdown pins low to turn off each VL53L0X
GPIO.output(sensor1_shutdown, GPIO.LOW)
GPIO.output(sensor2_shutdown, GPIO.LOW)
GPIO.output(sensor3_shutdown, GPIO.LOW)
GPIO.output(sensor4_shutdown, GPIO.LOW)
GPIO.output(sensor5_shutdown, GPIO.LOW)
GPIO.output(sensor6_shutdown, GPIO.LOW)
GPIO.output(sensor7_shutdown, GPIO.LOW)
GPIO.output(sensor8_shutdown, GPIO.LOW)

# Keep all low for 500 ms or so to make sure they reset
time.sleep(0.50)

# Create one object per VL53L0X passing the address to give to each
tof = VL53L0X.VL53L0X(address=0x2B)
tof1 = VL53L0X.VL53L0X(address=0x34)
tof2 = VL53L0X.VL53L0X(address=0x35)
tof3 = VL53L0X.VL53L0X(address=0x2E)
tof4 = VL53L0X.VL53L0X(address=0x30)
tof5 = VL53L0X.VL53L0X(address=0x31)
tof6 = VL53L0X.VL53L0X(address=0x32)
tof7 = VL53L0X.VL53L0X(address=0x33)

# Set shutdown pin high for the first VL53L0X, then call to start ranging
GPIO.output(sensor1_shutdown, GPIO.HIGH)
time.sleep(0.50)
tof.start_ranging(VL53L0X.VL53L0X_HIGH_SPEED_MODE)

# Set shutdown pin high for the second VL53L0X, then call to start ranging
GPIO.output(sensor2_shutdown, GPIO.HIGH)
time.sleep(0.50)
tof1.start_ranging(VL53L0X.VL53L0X_HIGH_SPEED_MODE)

# Set shutdown pin high for the third VL53L0X, then call to start ranging
GPIO.output(sensor3_shutdown, GPIO.HIGH)
time.sleep(0.50)
tof2.start_ranging(VL53L0X.VL53L0X_HIGH_SPEED_MODE)

# Set shutdown pin high for the fourth VL53L0X, then call to start ranging
GPIO.output(sensor4_shutdown, GPIO.HIGH)
time.sleep(0.50)
tof3.start_ranging(VL53L0X.VL53L0X_HIGH_SPEED_MODE)

# Set shutdown pin high for the fifth VL53L0X, then call to start ranging
GPIO.output(sensor5_shutdown, GPIO.HIGH)
time.sleep(0.50)
tof4.start_ranging(VL53L0X.VL53L0X_HIGH_SPEED_MODE)

# Set shutdown pin high for the sixth VL53L0X, then call to start ranging
GPIO.output(sensor6_shutdown, GPIO.HIGH)
time.sleep(0.50)
tof5.start_ranging(VL53L0X.VL53L0X_HIGH_SPEED_MODE)

# Set shutdown pin high for the seventh VL53L0X, then call to start ranging
GPIO.output(sensor7_shutdown, GPIO.HIGH)
time.sleep(0.50)
tof6.start_ranging(VL53L0X.VL53L0X_HIGH_SPEED_MODE)

# Set shutdown pin high for the eighth VL53L0X, then call to start ranging
GPIO.output(sensor8_shutdown, GPIO.HIGH)
time.sleep(0.50)
tof7.start_ranging(VL53L0X.VL53L0X_HIGH_SPEED_MODE)

timing = tof.get_timing()
if timing < 50000:
    timing = 50000
print("Timing %d ms" % (timing / 1000))

rospy.init_node('tof_publisher', anonymous=True)
pub = rospy.Publisher('tof_distances', Int32MultiArray, queue_size=10)

try:
    while not rospy.is_shutdown():
        # Read distance from each sensor
        distances = []

        distance = tof.get_distance()
        if distance > 0:
            distances.append(distance)

        distance = tof1.get_distance()
        if distance > 0:
            distances.append(distance)

        distance = tof2.get_distance()
        if distance > 0:
            distances.append(distance)

        distance = tof3.get_distance()
        if distance > 0:
            distances.append(distance)

        distance = tof4.get_distance()
        if distance > 0:
            distances.append(distance)

        distance = tof5.get_distance()
        if distance > 0:
            distances.append(distance)

        distance = tof6.get_distance()
        if distance > 0:
            distances.append(distance)

        distance = tof7.get_distance()
        if distance > 0:
            distances.append(distance)

        # Publish the distances as Int32MultiArray
        msg = Int32MultiArray(data=distances)
        pub.publish(msg)

        # Get the maximum timing value among all sensors
        timing = max(tof.get_timing(), tof1.get_timing(), tof2.get_timing(), tof3.get_timing(),
                     tof4.get_timing(), tof5.get_timing(), tof6.get_timing(), tof7.get_timing())

        time.sleep(timing / 1000000.00)

except KeyboardInterrupt:
    # Stop ranging and set shutdown pins low when program is interrupted
    tof.stop_ranging()
    GPIO.output(sensor1_shutdown, GPIO.LOW)

    tof1.stop_ranging()
    GPIO.output(sensor2_shutdown, GPIO.LOW)

    tof2.stop_ranging()
    GPIO.output(sensor3_shutdown, GPIO.LOW)

    tof3.stop_ranging()
    GPIO.output(sensor4_shutdown, GPIO.LOW)

    tof4.stop_ranging()
    GPIO.output(sensor5_shutdown, GPIO.LOW)

    tof5.stop_ranging()
    GPIO.output(sensor6_shutdown, GPIO.LOW)

    tof6.stop_ranging()
    GPIO.output(sensor7_shutdown, GPIO.LOW)

    tof7.stop_ranging()
    GPIO.output(sensor8_shutdown, GPIO.LOW)