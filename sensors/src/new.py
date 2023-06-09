#!/usr/bin/env python3

import time
import rospy
from std_msgs.msg import Int32MultiArray

import board
from digitalio import DigitalInOut
from adafruit_vl53l0x import VL53L0X

# declare the singleton variable for the default I2C bus
i2c = board.I2C()  # uses board.SCL and board.SDA

# declare the digital output pins connected to the "SHDN" pin on each VL53L0X sensor
xshut = [
    DigitalInOut(board.D12),
    DigitalInOut(board.D16),
    DigitalInOut(board.D13),
    DigitalInOut(board.D19),
    # add more VL53L0X sensors by defining their SHDN pins here
]

for power_pin in xshut:
    # make sure these pins are a digital output, not a digital input
    power_pin.switch_to_output(value=False)
    # These pins are active when Low, meaning:

# initialize a list to be used for the array of VL53L0X sensors
vl53 = []

# now change the addresses of the VL53L0X sensors
for i, power_pin in enumerate(xshut):
    # turn on the VL53L0X to allow hardware check
    power_pin.value = True
    # instantiate the VL53L0X sensor on the I2C bus & insert it into the "vl53" list
    vl53.insert(i, VL53L0X(i2c))  # also performs VL53L0X hardware check

    # start continuous mode
    vl53[i].start_continuous()

    # you will see the benefit of continuous mode if you set the measurement timing
    # budget very high.
    # vl53[i].measurement_timing_budget = 2000000

    # no need to change the address of the last VL53L0X sensor
    if i < len(xshut) - 1:
        # default address is 0x29. Change that to something else
        vl53[i].set_address(i + 0x30)  # address assigned should NOT be already in use


def publish_range():
    """Publish the sensor range readings as a ROS topic"""
    rospy.init_node('vl53l0x_publisher', anonymous=True)
    range_publisher = rospy.Publisher('sensor_ranges', Int32MultiArray, queue_size=10)
    rate = rospy.Rate(1)  # publish at 1 Hz

    while not rospy.is_shutdown():
        ranges = Int32MultiArray(data=[sensor.range for sensor in vl53])
        print(ranges)
        range_publisher.publish(ranges)
        rate.sleep()


def stop_continuous():
    """This is not required if you use XSHUT to reset the sensor unless you want to save some energy"""
    for sensor in vl53:
        sensor.stop_continuous()


if __name__ == "__main__":
    try:
        publish_range()
    except rospy.ROSInterruptException:
        pass
    finally:
        stop_continuous()
