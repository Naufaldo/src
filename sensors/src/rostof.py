#!/usr/bin/env python3

import time
from python.VL53L0X import VL53L0X
import RPi.GPIO as GPIO
import rospy
from std_msgs.msg import Float32MultiArray

# GPIO for Sensor 1 shutdown pin
sensor1_shutdown = 12
# GPIO for Sensor 2 shutdown pin
sensor2_shutdown = 16
# GPIO for Sensor 3 shutdown pin
sensor3_shutdown = 20
# GPIO for Sensor 4 shutdown pin
sensor4_shutdown = 21

GPIO.setwarnings(False)

# Setup GPIO for shutdown pins on each VL53L0X
GPIO.setmode(GPIO.BCM)
GPIO.setup(sensor1_shutdown, GPIO.OUT)
GPIO.setup(sensor2_shutdown, GPIO.OUT)
GPIO.setup(sensor3_shutdown, GPIO.OUT)
GPIO.setup(sensor4_shutdown, GPIO.OUT)

# Set all shutdown pins low to turn off each VL53L0X
GPIO.output(sensor1_shutdown, GPIO.LOW)
GPIO.output(sensor2_shutdown, GPIO.LOW)
GPIO.output(sensor3_shutdown, GPIO.LOW)
GPIO.output(sensor4_shutdown, GPIO.LOW)

# Keep all low for 500 ms or so to make sure they reset
time.sleep(0.50)

# Create one object per VL53L0X passing the address to give to each
tof = VL53L0X.VL53L0X(address=0x2B)
tof1 = VL53L0X.VL53L0X(address=0x2D)
tof2 = VL53L0X.VL53L0X(address=0x2F)
tof3 = VL53L0X.VL53L0X(address=0x2E)

# Set shutdown pin high for the first VL53L0X, then call to start ranging
GPIO.output(sensor1_shutdown, GPIO.HIGH)
time.sleep(0.50)
tof.start_ranging(VL53L0X.VL53L0X_BETTER_ACCURACY_MODE)

# Set shutdown pin high for the second VL53L0X, then call to start ranging
GPIO.output(sensor2_shutdown, GPIO.HIGH)
time.sleep(0.50)
tof1.start_ranging(VL53L0X.VL53L0X_BETTER_ACCURACY_MODE)

# Set shutdown pin high for the third VL53L0X, then call to start ranging
GPIO.output(sensor3_shutdown, GPIO.HIGH)
time.sleep(0.50)
tof2.start_ranging(VL53L0X.VL53L0X_BETTER_ACCURACY_MODE)

# Set shutdown pin high for the fourth VL53L0X, then call to start ranging
GPIO.output(sensor4_shutdown, GPIO.HIGH)
time.sleep(0.50)
tof3.start_ranging(VL53L0X.VL53L0X_BETTER_ACCURACY_MODE)

timing = tof.get_timing()
if timing < 20000:
    timing = 20000
print("Timing %d ms" % (timing / 1000))

rospy.init_node('tof_publisher', anonymous=True)
pub = rospy.Publisher('tof_distances', Float32MultiArray, queue_size=10)

try:
    while not rospy.is_shutdown():
        # Read distance from each sensor
        distances = []

        distance = tof.get_distance()
        if distance > 0:
            distances.append(distance / 10)

        distance = tof1.get_distance()
        if distance > 0:
            distances.append(distance / 10)

        distance = tof2.get_distance()
        if distance > 0:
            distances.append(distance / 10)

        distance = tof3.get_distance()
        if distance > 0:
            distances.append(distance / 10)

        # Publish the distances as Float32MultiArray
        msg = Float32MultiArray(data=distances)
        pub.publish(msg)

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
