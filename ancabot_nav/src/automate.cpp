#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <iostream>

using namespace std;

// Action specification for move_base
typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

int main(int argc, char** argv){

  // Connect to ROS
  ros::init(argc, argv, "simple_navigation_goals");

  // Tell the action client that we want to spin a thread by default
  MoveBaseClient ac("move_base", true);

  // Wait for the action server to come up so that we can begin processing goals.
  while(!ac.waitForServer(ros::Duration(5.0))){
    ROS_INFO("Waiting for the move_base action server to come up");
  }

  // Define the sequence of destinations
  double destinations[][3] = {
    {0.46674978733062744, 0.009915530681610107, 0.9995841865908485}, // Arah Korban 1
    {0.47411449790000916, 0.1285257339477539, 0.6555718605526236},    // Ambil Korban 1
    {2.4658145904541016, 0.19893264770507812, 0.9196588897926539},   // Simpen Korban 1
    {2.1826889514923096, 0.6185263991355896, 0.03423934984405933},    // Arah Korban 2
    // Add more destinations in the sequence
    {2.4959347248077393, 0.8430209755897522, 0.7359457450143778},     // Simpen korban kelereng
    {1.3070660829544067, 0.6406080722808838, 0.01420421912936522}     // Arah Tangga
  };
  int numDestinations = sizeof(destinations) / sizeof(destinations[0]);

  for(int i = 0; i < numDestinations; i++) {
    // Create a new goal to send to move_base
    move_base_msgs::MoveBaseGoal goal;

    // Set the goal position and orientation based on the current destination
    goal.target_pose.header.frame_id = "map";
    goal.target_pose.header.stamp = ros::Time::now();
    goal.target_pose.pose.position.x = destinations[i][0];
    goal.target_pose.pose.position.y = destinations[i][1];
    goal.target_pose.pose.orientation.w = destinations[i][2];

    ROS_INFO("Sending goal");
    ac.sendGoal(goal);

    // Wait until the robot reaches the goal
    ac.waitForResult();

    if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
      ROS_INFO("The robot has arrived at the goal location");
    else
      ROS_INFO("The robot failed to reach the goal location for some reason");
  }

  cout << "\nAll destinations visited. Exiting..." << endl;

  return 0;
}
