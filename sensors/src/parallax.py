#!/usr/bin/env python3
import RPi.GPIO as GPIO
import time
import rospy
from std_msgs.msg import Float32MultiArray

def sensor_node():
    rospy.init_node('sensor_node', anonymous=True)
    pub = rospy.Publisher('sensor_data', Float32MultiArray, queue_size=10)
    rate = rospy.Rate(10)  # 10Hz publishing rate

    while not rospy.is_shutdown():
        sensor_data = read_sensor_data()

        # Publish the sensor data as a Float32MultiArray message
        pub.publish(Float32MultiArray(data=sensor_data))
        rate.sleep()

def read_sensor_data():
    sensor_data = []

    # Assign GPIO pins for each sensor
    GPIO_SIG = [14, 15, 18, 23, 24]  # Modify these pins based on your wiring
    #kanan depan , kanan belakang, tengah belakang, kiri belakang, kiri depan

    # Set up GPIO mode
    GPIO.setmode(GPIO.BCM)

    for pin in GPIO_SIG:
        # Measure distance for each sensor
        GPIO.setup(pin, GPIO.OUT)
        GPIO.output(pin, 0)

        time.sleep(0.000002)

        # Send trigger signal
        GPIO.output(pin, 1)

        time.sleep(0.000005)

        GPIO.output(pin, 0)

        GPIO.setup(pin, GPIO.IN)

        while GPIO.input(pin) == 0:
            starttime = time.time()

        while GPIO.input(pin) == 1:
            endtime = time.time()

        duration = endtime - starttime
        # Distance is defined as time/2 (there and back) * speed of sound 34000 cm/s
        distance = (duration * 34000) / 2

        sensor_data.append(distance)

    GPIO.cleanup()

    return sensor_data

if __name__ == '__main__':
    try:
        sensor_node()
    except rospy.ROSInterruptException:
        pass


