#!/usr/bin/env python3

import time
import sys
sys.path.append('/home/pi/VL53L0X_rasp_python/python')
import VL53L0X
import rospy
from std_msgs.msg import Int32MultiArray
import Adafruit_SSD1306
from PIL import Image, ImageDraw, ImageFont
import board
import adafruit_tca9548a

def publish_distances(distances):
    # Publish the distances as Int32MultiArray
    msg = Int32MultiArray(data=distances)
    pub.publish(msg)

    # Clear the OLED display and draw the distances
    draw.rectangle((0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT), outline=0, fill=0)
    for i, distance in enumerate(distances):
        text = "Sensor {}: {} mm".format(i+1, distance)
        draw.text((0, i*10), text, font=font, fill=255)
    display.image(image)
    display.display()

def display_error(error_message):
    # Clear the OLED display and draw the error message
    draw.rectangle((0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT), outline=0, fill=0)
    draw.text((0, 0), "Error:", font=font, fill=255)
    draw.text((0, 10), error_message, font=font, fill=255)
    display.image(image)
    display.display()

    # Log the error message using rospy.loginfo
    rospy.loginfo("Error: {}".format(error_message))

rospy.init_node('tof_publisher', anonymous=True)
pub = rospy.Publisher('tof_distances', Int32MultiArray, queue_size=10)

# Create a list to store the VL53L0X objects for each sensor
sensors = []

# Create an instance of the TCA9548A multiplexer
i2c = board.I2C()
tca = adafruit_tca9548a.TCA9548A(i2c)


# Create VL53L0X objects for devices on TCA9548A bus 4
for i in range(4):
    sensor = VL53L0X.VL53L0X(i2c_bus=4)  # Use the appropriate I2C bus number
    sensor.start_ranging(VL53L0X.VL53L0X_BETTER_ACCURACY_MODE)
    sensors.append(sensor)

# Get the timing from the first sensor
timing = sensors[0].get_timing()
if timing < 100000:
    timing = 100000
print("Timing %d ms" % (timing / 1000))

# OLED display resolution
DISPLAY_WIDTH = 128
DISPLAY_HEIGHT = 64

# Create an instance of the Adafruit SSD1306 OLED display
display = Adafruit_SSD1306.SSD1306_128_64(rst=RST, i2c_bus=tca[4])  # Use the appropriate I2C bus number
display.begin()
display.clear()
display.display()
image = Image.new('1', (DISPLAY_WIDTH, DISPLAY_HEIGHT))
draw = ImageDraw.Draw(image)
font = ImageFont.load_default()

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
                    error_message = "Invalid distance from sensor {}".format(i+1)
                    display_error(error_message)
                    print("Error: " + error_message)
            except Exception as e:
                error_message = "Failed to get distance from sensor {}: {}".format(i+1, str(e))
                display_error(error_message)
                print("Error: " + error_message)

        # Check if all 4 sensors have published their distances
        if len(distances) == 4:
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
