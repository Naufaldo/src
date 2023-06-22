#!/usr/bin/env python3

import time
import sys
sys.path.append('/home/pi/VL53L0X_rasp_python/python')
import VL53L0X
import rospy
from std_msgs.msg import Int32MultiArray
from sensor_msgs.msg import Imu

from PIL import Image
from PIL import ImageDraw
from PIL import ImageFont

# Import the required libraries for the GME12864 display using I2C
import Adafruit_GPIO.I2C as I2C
import Adafruit_SSD1306

# Define display constants
DISPLAY_WIDTH = 128
DISPLAY_HEIGHT = 64

# Initialize the display using I2C with the specified parameters
RST = None  # If you have a reset pin, specify it here
disp = Adafruit_SSD1306.SSD1306_128_64(rst=RST, i2c_address=0x3C)
disp.begin()
disp.clear()
disp.display()

# Create a blank image for drawing
image = Image.new('1', (DISPLAY_WIDTH, DISPLAY_HEIGHT))
draw = ImageDraw.Draw(image)

# Define font
font = ImageFont.load_default()

def publish_distances(distances):
    # Publish the distances as Int32MultiArray
    msg = Int32MultiArray(data=distances)
    pub.publish(msg)

def display_distances(sensor_distances, orientation_z):
    # Clear the image
    draw.rectangle((0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT), outline=0, fill=0)

    # Display the sensor array
    array_text = 'Sensor Array:\n{}'.format(sensor_distances)
    draw.text((0, 0), array_text, font=font, fill=255)

    # Display the orientation Z
    orientation_text = 'Orientation Z: {}'.format(orientation_z)
    draw.text((0, 15), orientation_text, font=font, fill=255)

    # Display the image
    disp.image(image)
    disp.display()

def display_error(sensor_index, orientation_z):
    # Calculate the top position for displaying the error message
    top = 30  # Position below the gyro data

    # Clear the image
    draw.rectangle((0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT), outline=0, fill=0)

    # Display the sensor array
    array_text = 'Sensor Array:\n{}'.format(sensor_distances)
    draw.text((0, 0), array_text, font=font, fill=255)

    # Display the orientation Z
    orientation_text = 'Orientation Z: {}'.format(orientation_z)
    draw.text((0, 15), orientation_text, font=font, fill=255)

    # Display the sensor index and error message
    error_text = 'Sensor {}: Error'.format(sensor_index)
    draw.text((0, top), error_text, font=font, fill=255)

    # Display the image
    disp.image(image)
    disp.display()

def imu_callback(data):
    # Extract the orientation data (z) from the IMU message
    orientation_z = data.orientation.z

    # Display the distances and orientation Z
    display_distances(sensor_distances, orientation_z)

rospy.init_node('tof_publisher', anonymous=True)
pub = rospy.Publisher('tof_distances', Int32MultiArray, queue_size=10)

# Subscribe to the IMU data topic
rospy.Subscriber('imu/data', Imu, imu_callback)

# Create a list to store the VL53L0X objects for each sensor
sensors = []

# Create VL53L0X objects for devices on TCA9548A buses 0 to 7
for i in range(4):
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
        sensor_distances = []
        has_error = False

        # Get distances from each sensor
        for i, sensor in enumerate(sensors):
            try:
                distance = sensor.get_distance()
                if distance > 0:
                    sensor_distances.append(distance)
                else:
                    print("Error: Invalid distance value from sensor %d" % i)
                    has_error = True
            except Exception as e:
                print("Error: Failed to get distance from sensor %d. Exception: %s" % (i, str(e)))
                has_error = True

        # Display the distances if no error occurred
        if not has_error:
            display_distances(sensor_distances, 0)  # Provide a default value for orientation Z
        else:
            # Display sensor distances before showing the error message
            display_distances(sensor_distances, 0)  # Provide a default value for orientation Z
            display_error(len(sensors), 0)  # Provide a default value for orientation Z

        # Check if all sensors have published their distances
        if len(sensor_distances) == len(sensors):
            publish_distances(sensor_distances)
        else:
            print("Error: Failed to get distances from all sensors")
            publish_distances(sensor_distances)

        time.sleep(timing / 1000000.00)
except KeyboardInterrupt:
    # Stop ranging for all sensors when the program is interrupted
    for sensor in sensors:
        sensor.stop_ranging()

    rospy.loginfo("Program interrupted")

# Stop ranging for all sensors before exiting
for sensor in sensors:
    sensor.stop_ranging()
