/*
 * Author: Automatic Addison
 * Date: May 30, 2021
 * ROS Version: ROS 1 - Melodic
 * Website: https://automaticaddison.com
 * This ROS node sends the robot goals to move to a particular location on 
 * a map. I have configured this program to the map of my own apartment.
 *
 * 1 = Bathroom
 * 2 = Bedroom
 * 3 = Front Door
 * 4 = Living Room
 * 5 = Home Office
 * 6 = Kitchen (Default)
 */
 
 /*

 initialpose home :

 position: 
      x: 0.029102548956871033
      y: -0.006215959787368774
      z: 0.0
    orientation: 
      x: 0.0
      y: 0.0
      z: 0.028834942647973153
      w: 0.9995841865908485
  covariance: [0.25, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.25, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.06853892326654787]

  ambil korban 1: 
position: 
        x: 0.48411449790000916
        y: 0.1285257339477539
        z: 0.0
      orientation: 
        x: 0.0
        y: 0.0
        z: 0.7257380700164059
        w: 0.6879711140221385

nyimpen korban 1: 
position: 
        x: 2.4658145904541016
        y: 0.19893264770507812
        z: 0.0
      orientation: 
        x: 0.0
        y: 0.0
        z: -0.3927181259190151
        w: 0.9196588897926539

Ngadep Kelereng :
position: 
        x: 2.1826889514923096
        y: 0.6185263991355896
        z: 0.0
      orientation: 
        x: 0.0
        y: 0.0
        z: -0.9994136615647478
        w: 0.03423934984405933

nyimpen korban kelereng : 
x: 2.4959347248077393
        y: 0.8430209755897522
        z: 0.0
      orientation: 
        x: 0.0
        y: 0.0
        z: 0.6770405160662339
        w: 0.7359457450143778

Arah Tangga :
position: 
        x: 1.3070660829544067
        y: 0.6406080722808838
        z: 0.0
      orientation: 
        x: 0.0
        y: 0.0
        z: 0.9998991149905699
        w: 0.01420421912936522



value parameter poin setiap posisi
Arah ke korban 1 A:
  x: 0.46674978733062744
  y: 0.009915530681610107
  z: -0.0007991790771484375

Ambil Korban 1 B:
x: 0.481564998626709
  y: 0.2181706428527832
  z: 0.0037766098976135254
Simpen korban 1 C :
  x: 2.5189831256866455
  y: 0.15150058269500732
  z: 0.0011653304100036621
Pergi Arah kelereng D:
 x: 2.4336533546447754
  y: 0.5878385305404663
  z: 0.003141045570373535
Menghadap Kelereng E:
  x: 2.2450156211853027
  y: 0.609586238861084
  z: 0.0028699636459350586
---
F:
  x: 2.4735209941864014
  y: 0.9661422967910767
  z: 0.003865063190460205

G:
 x: 1.3552519083023071
  y: 0.6135255098342896
  z: 0.004418373107910156

 */
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
 
  //tell the action client that we want to spin a thread by default
  MoveBaseClient ac("move_base", true);
 
  // Wait for the action server to come up so that we can begin processing goals.
  while(!ac.waitForServer(ros::Duration(5.0))){
    ROS_INFO("Waiting for the move_base action server to come up");
  }
 
  int user_choice = 6;
  char choice_to_continue = 'Y';
  bool run = true;
     
  while(run) {
 
    // Ask the user where he wants the robot to go?
    cout << "\nWhere do you want the robot to go?" << endl;
    cout << "\n1 = Bathroom" << endl;
    cout << "2 = Bedroom" << endl;
    cout << "3 = Front Door" << endl;
    cout << "4 = Living Room" << endl;
    cout << "5 = Home Office" << endl;
    cout << "6 = Kitchen" << endl;
    cout << "\nEnter a number: ";
    cin >> user_choice;
 
    // Create a new goal to send to move_base 
    move_base_msgs::MoveBaseGoal goal;
 
    // Send a goal to the robot
    goal.target_pose.header.frame_id = "map";
    goal.target_pose.header.stamp = ros::Time::now();
         
    bool valid_selection = true;
 
    // Use map_server to load the map of the environment on the /map topic. 
    // Launch RViz and click the Publish Point button in RViz to 
    // display the coordinates to the /clicked_point topic.
    switch (user_choice) {
      case 1:
        cout << "\nGoal Location: Bathroom\n" << endl;
        goal.target_pose.pose.position.x = 10.0;
    goal.target_pose.pose.position.y = 3.7;
        goal.target_pose.pose.orientation.w = 1.0;
        break;
      case 2:
        cout << "\nGoal Location: Bedroom\n" << endl;
        goal.target_pose.pose.position.x = 8.1;
    goal.target_pose.pose.position.y = 4.3;
        goal.target_pose.pose.orientation.w = 1.0;
        break;
      case 3:
        cout << "\nGoal Location: Front Door\n" << endl;
        goal.target_pose.pose.position.x = 10.5;
    goal.target_pose.pose.position.y = 2.0;
        goal.target_pose.pose.orientation.w = 1.0;
        break;
      case 4:
        cout << "\nGoal Location: Living Room\n" << endl;
        goal.target_pose.pose.position.x = 5.3;
    goal.target_pose.pose.position.y = 2.7;
        goal.target_pose.pose.orientation.w = 1.0;
        break;
      case 5:
        cout << "\nGoal Location: Home Office\n" << endl;
        goal.target_pose.pose.position.x = 2.5;
    goal.target_pose.pose.position.y = 2.0;
        goal.target_pose.pose.orientation.w = 1.0;
        break;
      case 6:
        cout << "\nGoal Location: Kitchen\n" << endl;
        goal.target_pose.pose.position.x = 3.0;
    goal.target_pose.pose.position.y = 6.0;
        goal.target_pose.pose.orientation.w = 1.0;
        break;
      default:
        cout << "\nInvalid selection. Please try again.\n" << endl;
        valid_selection = false;
    }       
         
    // Go back to beginning if the selection is invalid.
    if(!valid_selection) {
      continue;
    }
 
    ROS_INFO("Sending goal");
    ac.sendGoal(goal);
 
    // Wait until the robot reaches the goal
    ac.waitForResult();
 
    if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
      ROS_INFO("The robot has arrived at the goal location");
    else
      ROS_INFO("The robot failed to reach the goal location for some reason");
         
    // Ask the user if he wants to continue giving goals
    do {
      cout << "\nWould you like to go to another destination? (Y/N)" << endl;
      cin >> choice_to_continue;
      choice_to_continue = tolower(choice_to_continue); // Put your letter to its lower case
    } while (choice_to_continue != 'n' && choice_to_continue != 'y'); 
 
    if(choice_to_continue =='n') {
        run = false;
    }  
  }
   
  return 0;
}