#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <std_msgs/Bool.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <geometry_msgs/PoseWithCovarianceStamped.h>

using namespace std;

// Action specification for move_base
typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

int main(int argc, char** argv) {
  // Initialize ROS
  ros::init(argc, argv, "simple_navigation_goals");

  // Create a ROS node handle
  ros::NodeHandle nh;

  // Create a publisher to publish the initial pose
  ros::Publisher initialPosePub = nh.advertise<geometry_msgs::PoseWithCovarianceStamped>("initialpose", 1);
  ros::Publisher headPub = nh.advertise<geometry_msgs::Twist>("/head_Tws", 1);
  ros::Publisher legHeightPub = nh.advertise<std_msgs::Bool>("/leg", 1);

  // Create the initial pose message
  geometry_msgs::PoseWithCovarianceStamped initialPoseMsg;
  initialPoseMsg.header.frame_id = "map";
  initialPoseMsg.pose.pose.position.x = 0.029102548956871033;
  initialPoseMsg.pose.pose.position.y = -0.006215959787368774;
  initialPoseMsg.pose.pose.position.z = 0.0;
  initialPoseMsg.pose.pose.orientation.x = 0.0;
  initialPoseMsg.pose.pose.orientation.y = 0.0;
  initialPoseMsg.pose.pose.orientation.z = 0.028834942647973153;
  initialPoseMsg.pose.pose.orientation.w = 0.9995841865908485;
  initialPoseMsg.pose.covariance = {
    0.25, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.25, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.06853892326654787
  };

  // Publish the initial pose
  initialPosePub.publish(initialPoseMsg);

  // Connect to the move_base action server
  MoveBaseClient ac("move_base", true);

  // Wait for the action server to come up
  while (!ac.waitForServer(ros::Duration(5.0))) {
    ROS_INFO("Waiting for the move_base action server to come up");
  }

  // Define the sequence of destinations
  double destinations[][6] = {
/*
      seq: 0
  stamp: 
    secs: 1690269012
    nsecs: 432677040
  frame_id: "map"
pose: 
  position: 
    x: 0.48933225870132446
    y: 0.006300929468125105
    z: 0.0
  orientation: 
    x: 0.0
    y: 0.0
    z: 0.006688636727631092
    w: 0.0

    ---
    header: 
  seq: 1
  stamp: 
    secs: 1690269411
    nsecs: 432564649
  frame_id: "map"
pose: 
  position: 
    x: 2.384226083755493
    y: 0.010871904902160168
    z: 0.0
  orientation: 
    x: 0.0
    y: 0.0
    z: -0.5791836977005006
    w: 0.0

---
header: 
  seq: 2
  stamp: 
    secs: 1690269669
    nsecs: 931631464
  frame_id: "map"
pose: 
  position: 
    x: 2.374901533126831
    y: 0.7160121202468872
    z: 0.0
  orientation: 
    x: 0.0
    y: 0.0
    z: 1.4213865995407104
    w: 0.0
---


*/
    // {x, y, w, Lifter, Gripper, leg_height}
    {0.46674978733062744, 0.009915530681610107, 0.9995841865908485, -2, 0, 0}, // Arah Korban 1
    {0.46674978733062744, 0.009915530681610107, 0.6555718605526236, -2, 0, 0}, // Arah Korban 1
    {0.47411449790000916, 0.1285257339477539, 0.6555718605526236, 0, -1, 1},    // Ambil Korban 1
    {2.4658145904541016, 0.19893264770507812, 0.9196588897926539, -2, 0, 1},   // Simpen Korban 1
    {2.1826889514923096, 0.6185263991355896, 0.03423934984405933, -2, 0, 1},    // Arah Korban 2
    // Add more destinations in the sequence
    {2.4959347248077393, 0.8430209755897522, 0.7359457450143778, -2, 0, 1},     // Simpen korban kelereng
    {1.3070660829544067, 0.6406080722808838, 0.01420421912936522, -2, 0, 1}     // Arah Tangga
  };
  int numDestinations = sizeof(destinations) / sizeof(destinations[0]);

  for (int i = 0; i < numDestinations; i++) {
    // Create a new goal to send to move_base
    move_base_msgs::MoveBaseGoal goal;

    // Set the goal position and orientation based on the current destination
    goal.target_pose.header.frame_id = "map";
    goal.target_pose.header.stamp = ros::Time::now();
    goal.target_pose.pose.position.x = destinations[i][0];
    goal.target_pose.pose.position.y = destinations[i][1];
    goal.target_pose.pose.orientation.w = destinations[i][2];

    // Create and set the head and leg height messages
    geometry_msgs::Twist headTws;
    headTws.linear.x = destinations[i][3] * 0.625;
    headTws.linear.y = destinations[i][4] * 1.1;
    std_msgs::Bool legHeight;
    legHeight.data = destinations[i][5];

    ROS_INFO("Sending goal");
    ac.sendGoal(goal);

    // Publish the head and leg height commands
    headPub.publish(headTws);
    legHeightPub.publish(legHeight);

    // Wait until the robot reaches the goal
    ac.waitForResult();

    if (ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
      ROS_INFO("The robot has arrived at the goal location");
    else
      ROS_INFO("The robot failed to reach the goal location for some reason");
  }

  cout << "\nAll destinations visited. Exiting..." << endl;

  return 0;
}
