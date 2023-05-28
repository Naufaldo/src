#include <ros.h>
#include <std_msgs/Int32.h>
#include <NewPing.h>

ros::NodeHandle nh;
std_msgs::Int32 ping_1;
std_msgs::Int32 ping_2;
std_msgs::Int32 ping_3;
std_msgs::Int32 ping_4;
std_msgs::Int32 ping_5;
ros::Publisher ping_1_pub("ping_1_topic", &ping_1);
ros::Publisher ping_2_pub("ping_2_topic", &ping_2);
ros::Publisher ping_3_pub("ping_3_topic", &ping_3);
ros::Publisher ping_4_pub("ping_4_topic", &ping_4);
ros::Publisher ping_5_pub("ping_5_topic", &ping_5);

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
  Serial.begin(115200);
  nh.getHardware()->setBaud(115200); // Set the baud rate to 115200
  nh.initNode();
  nh.advertise(ping_1_pub);
  nh.advertise(ping_2_pub);
  nh.advertise(ping_3_pub);
  nh.advertise(ping_4_pub);
  nh.advertise(ping_5_pub);

  pingTimer[0] = millis() + 75;                       // First ping starts at 75ms, gives time for the Arduino to chill before starting.
  for (uint8_t i = 1; i < PING_PIN; i++)              // Set the starting time for each sensor.
    pingTimer[i] = pingTimer[i - 1] + PING_INTERVAL;
}

void loop() {
  for (uint8_t i = 0; i < PING_PIN; i++) {                            // Loop through all the sensors.
    if (millis() >= pingTimer[i]) {                                   // Is it this sensor's time to ping?
      pingTimer[i] += PING_INTERVAL * PING_PIN;                       // Set next time this sensor will be pinged.
      ping[currentSensor].timer_stop();                               // Make sure previous timer is canceled before starting a new ping (insurance).
      currentSensor = i;                                              // Sensor being accessed.
      cm[currentSensor] = 0;                                          // Make distance zero in case there's no ping echo for this sensor.
      ping[currentSensor].ping_timer(echoCheck);                      // Do the ping (processing continues, interrupt will call echoCheck to look for echo).
      if (currentSensor == 0 && i == PING_PIN - 1) {                   // Sensor ping cycle complete, publish all the data.
        ping_1.data = cm[0];
        ping_2.data = cm[1];
        ping_3.data = cm[2];
        ping_4.data = cm[3];
        ping_5.data = cm[4];
        ping_1_pub.publish(&ping_1);
        ping_2_pub.publish(&ping_2);
        ping_3_pub.publish(&ping_3);
        ping_4_pub.publish(&ping_4);
        ping_5_pub.publish(&ping_5);
      }
    }
  }

  nh.spinOnce();
}

void echoCheck() {                                                          // If ping received, set the sensor distance to array.
  if (ping[currentSensor].check_timer())
    cm[currentSensor] = ping[currentSensor].ping_result / US_ROUNDTRIP_CM;
}
