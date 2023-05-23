#include <NewPing.h>
#include <ros.h>
#include <std_msgs/Float32MultiArray.h>

ros::NodeHandle nh;
std_msgs::Float32MultiArray sensor_array_msg;
ros::Publisher sensor_array_pub("sensor_array_topic", &sensor_array_msg);

#define PING_PIN     5    // Number of sensors.
#define MAX_DISTANCE 500  // Maximum distance (in cm) to ping.
#define PING_INTERVAL 33  // Milliseconds between sensor pings (29ms is about the min to avoid cross-sensor echo).

unsigned long pingTimer[PING_PIN]; // Holds the times when the next ping should happen for each sensor.
unsigned int cm[PING_PIN];         // Where the ping distances are stored.
uint8_t currentSensor = 0;         // Keeps track of which sensor is active.


NewPing ping[PING_PIN] = {        // Sensor object array.
  NewPing(8, 8, MAX_DISTANCE),    // Upper-Left Sensor
  NewPing(9, 9, MAX_DISTANCE),    // Bottom-Left Sensor
  NewPing(10, 10, MAX_DISTANCE),  // Back Sensor
  NewPing(11, 11, MAX_DISTANCE),  // Bottom-Right Sensor
  NewPing(12, 12, MAX_DISTANCE)   // Upper-Right Sensor
};

void setup() {
  nh.initNode();
  nh.advertise(sensor_array_pub);

  pingTimer[0] = millis() + 75;                       // First ping starts at 75ms, gives time for the Arduino to chill before starting.
  for (uint8_t i = 1; i < PING_PIN; i++)              // Set the starting time for each sensor.
    pingTimer[i] = pingTimer[i - 1] + PING_INTERVAL;
}

void loop() {
  for (uint8_t i = 0; i < PING_PIN; i++) {                            // Loop through all the sensors.
    if (millis() >= pingTimer[i]) {                                   // Is it this sensor's time to ping?
      pingTimer[i] += PING_INTERVAL * PING_PIN;                       // Set next time this sensor will be pinged.
      if (i == 0 && currentSensor == PING_PIN - 1) oneSensorCycle();  // Sensor ping cycle complete, do something with the results.
      ping[currentSensor].timer_stop();                              // Make sure previous timer is canceled before starting a new ping (insurance).
      currentSensor = i;                                              // Sensor being accessed.
      cm[currentSensor] = 0;                                          // Make distance zero in case there's no ping echo for this sensor.
      ping[currentSensor].ping_timer(echoCheck);                     // Do the ping (processing continues, interrupt will call echoCheck to look for echo).
    }
  }

  sensor_array_pub.publish(&sensor_array_msg);
  nh.spinOnce();
}

void echoCheck() {                                                          // If ping received, set the sensor distance to array.
  if (ping[currentSensor].check_timer())
    cm[currentSensor] = ping[currentSensor].ping_result / US_ROUNDTRIP_CM;
}

void oneSensorCycle() {                     // Sensor ping cycle complete, do something with the results.
  String SensorPos; 
  for (uint8_t i = 0; i < PING_PIN; i++) {
    sensor_array_msg.data[i] = cm[i]
}