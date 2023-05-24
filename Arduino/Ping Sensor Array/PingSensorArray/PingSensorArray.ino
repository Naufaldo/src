#include <ros.h>
#include <std_msgs/Float32MultiArray.h>

ros::NodeHandle nh;
std_msgs::Float32MultiArray pingData;
ros::Publisher pingPub("ping_data", &pingData);

const int numSensors = 5;
int pingPins[numSensors] = {8, 9, 10, 11, 12};

void setup()
{
  nh.initNode();
  nh.advertise(pingPub);

  pingData.data_length = numSensors;
  pingData.data = new float[numSensors];
}

void loop()
{
  for (int i = 0; i < numSensors; i++)
  {
    pinMode(pingPins[i], OUTPUT);
    digitalWrite(pingPins[i], LOW);
    delayMicroseconds(2);
    digitalWrite(pingPins[i], HIGH);
    delayMicroseconds(10);
    digitalWrite(pingPins[i], LOW);
    pinMode(pingPins[i], INPUT);

    long duration = pulseIn(pingPins[i], HIGH);
    float distance = duration * 0.034 / 2.0; // Calculate distance in centimeters

    pingData.data[i] = distance;
  }

  pingPub.publish(&pingData);
  nh.spinOnce();

  delay(100); // Publish rate
}
