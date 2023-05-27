#include "ros/ros.h"
#include "std_msgs/String.h"

#include <nav_msgs/Odometry.h>
#include <hexapod_msgs/MergedPingArray.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/AccelStamped.h>
#include <std_msgs/Bool.h>
#include <std_msgs/UInt16.h>
#include <std_msgs/Float32.h>
#include <std_msgs/String.h>
#include <sensor_msgs/Imu.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <map>

int ping[5]={0,0,0,0,0};
// Depan kanan, Belakang Kanan, Belakang, Belakang kiri, depan kiri
void mergedPingCallback(const hexapod_msgs::MergedPingArray::ConstPtr& msg)
{
  for (int i=0;i<5;i++){
    ping[i]=msg->merged_ping_array[i];
  }
  ROS_INFO("I heard: [%d]", ping[0]);
}



float xaa[5],yaa[5],xas[5];
bool ff1,ff2,ff3;
int flag1=1;


// Map for movement keys
std::map<char, std::vector<float>> moveBindings{
    //Moving and Rotating
    {'q', {1, 0, 0, 1}},
    {'w', {1, 0, 0, 0}},
    {'e', {1, 0, 0, -1}},
    {'a', {0, 0, 0, 1}},
    {'s', {0, 0, 0, 0}},
    {'d', {0, 0, 0, -1}},
    {'z', {-1, 0, 0, -1}},
    {'x', {-1, 0, 0, 0}},
    {'c', {-1, 0, 0, 1}},
    //Holomonic Move
    {'Q', {1, -1, 0, 0}},
    {'W', {1, 0, 0, 0}},
    {'E', {1, 1, 0, 0}},
    {'A', {0, -1, 0, 0}},
    {'S', {0, 0, 0, 0}},
    {'D', {0, 1, 0, 0}},
    {'Z', {-1, -1, 0, 0}},
    {'X', {-1, 0, 0, 0}},
    {'C', {-1, 1, 0, 0}}};

// Map for movement keys
// std::map<char, std::vector<float>> moveBindings{
//     //Moving and Rotating
//     {'q', {1, 0, 0, 1, 0, 0}},
//     {'w', {1, 0, 0, 0, 0, 0}},
//     {'e', {1, 0, 0, -1, 0, 0}},
//     {'a', {0, 0, 0, 1, 0, 0}},
//     {'s', {0, 0, 0, 0, 0, 0}},
//     {'d', {0, 0, 0, -1, 0, 0}},
//     {'z', {-1, 0, 0, -1, 0, 0}},
//     {'x', {-1, 0, 0, 0, 0, 0}},
//     {'c', {-1, 0, 0, 1, 0, 0}},
//     //Holomonic Move
//     {'Q', {1, -1, 0, 0, 0, 0}},
//     {'W', {1, 0, 0, 0, 0, 0}},
//     {'E', {1, 1, 0, 0, 0, 0}},
//     {'A', {0, -1, 0, 0, 0, 0}},
//     {'S', {0, 0, 0, 0, 0, 0}},
//     {'D', {0, 1, 0, 0, 0, 0}},
//     {'Z', {-1, -1, 0, 0, 0, 0}},
//     {'X', {-1, 0, 0, 0, 0, 0}},
//     {'C', {-1, 1, 0, 0, 0, 0}}};
    // //Head Manipulating
    //  {'o', {0, 0, 0, 0, 0, 0}},
    // {'p', {0, 0, 0, 0, -2, 0}},
    // {'l', {0, 0, 0, 0, 0, -1}}};


//step
char a_gerak[]  ={'D','w','s','x','d','w','d','a','w','a','w','s','d','w','s','a'};

int gerak_1_[]={0,0,0,0,0,0,0,0,0};


//program buat limit sensor dan gerakan kaki dan juga gerakan gripper
std::map<int, std::vector<int>> step{
  // {1, {0,0,-2,0,0,0,0,0,0.5,0.5}},   //batas 0-7, speed, turn  //rotate kanan
  // Penejlasan {urutan gerakan , {lmit sensor 1,2,3,4,5 , nilai gripper x , nilai gripper y}}
  //dua terakhir -2,0 = P ; 0, -1 = l ; 0,0=o
  {0, {34, 62, 10, 10, 10, -2, 0}}, // posisi home gerak ke kanan
  {1, {260, 320, 5, 32, 57, 0, -1}}, // posisi depan K1
  {2, {65, 320, 21, 58, 58, -2, 0}}, // posisi k1
  {3, {304, 320, 13, 59, 59, 0, 0}}, // posisi depan k1
  {4, {15, 15, 59, 29, 29, 0, 0}}, // posisi hadap jalan retak
  {5, {15, 16, 28, 20, 28, 0, 0}}, // posisi sz 1
  {6, {31, 320, 176, 17, 30, 0, -1}}, // posisi meletakan sz 1
  {7, {16, 17, 15, 37, 43, -2, 0}}, // menghadap kelereng
  {8, {16, 18, 45, 17, 26, 0, -1}}, // posisi menghadap k2
  {9, {50, 51, 15, 20, 55, 0, 0}}, // posisi depan k2
  {10, {28, 55, 23, 18, 54, -2, 0}}, // posisi angkat k2
  {11, {20, 320, 58, 28, 34, 0, 0}}, // menghadap sz 2
  {12, {14, 121, 42, 31, 43, 0, -1}}, // menaruh k2
  {13, {19, 20, 23, 85, 40, -2, 0}}, // keluar kelereng
  
};
std::map<int, std::vector<bool>> _f_{
  // ini program untuk kondisi if 1 atau 0 (komparasi)
  // {1, {0,0,1,0,0,0,0,0,0}},  //kompar 0-7 (0)(L>=b) (1)(L<=b), LaserOrOdom(1=lase && 0=odom) //odom
  {0, {1,1,0,0,0,1}}, // posisi home gerak ke kanan semua sensor nilai lebih dari batas
  {1, {0,0,0,0,0,1}},
  {2, {0,0,0,0,0,1}},
  {3, {0,0,0,0,0,1}},
  {4, {0,0,0,0,0,1}},
  {5, {0,0,0,0,0,1}},
  {6, {0,0,0,0,0,1}},
  {7, {0,0,0,0,0,1}},
  {8, {0,0,0,0,0,1}},
  {9, {0,0,0,0,0,1}},
  {10, {0,0,0,0,0,1}},
  {11, {0,0,0,0,0,1}},
  {12, {0,0,0,0,0,1}},
  {13, {0,0,0,0,0,1}},

};


// Init variables
float speed(1);                                                 // Linear velocity (m/s)
float turn(1);                                                  // Angular velocity (rad/s)
float x(0), y(0), z(0), xa(0), ya(0), za(0), xb(0), yb(0), th(0); // Forward/backward/neutral direction vars
char key(' ');
geometry_msgs::Twist twist;
geometry_msgs::Twist head_Tws;
std_msgs::Bool imu_override_;
std_msgs::Bool leg_height_;
std_msgs::Bool state_;



bool pilih;
void kontrol(char arah_, int step_){
  key=arah_;
  int batas[5];
  if (step.count(step_) == 1)
    {
      for(int a=0;a<5;a++){
        batas[a]=step[step_][a];
      }
      xb=step[step_][5];
      yb=step[step_][6];
    }

  bool flag_[5];
  if (_f_.count(step_) == 1)
    {
      for(int a=0;a<5;a++){
        flag_[a]=_f_[step_][a];
      }
    pilih=_f_[step_][5];
    }
    

  if (moveBindings.count(key) == 1)
    {
      // Grab the direction data
      x = moveBindings[key][0];
      y = moveBindings[key][1];
      z = moveBindings[key][2];
      th = moveBindings[key][3];
      // xb = moveBindings[key][4];
      // yb = moveBindings[key][5];
      
      ROS_INFO("\rCurrent: speed %f   | turn %f | Last command: %c   ", speed, turn, key);
    }

    // Update the Twist message
    twist.linear.x = x * speed;
    twist.linear.y = y * speed;
    twist.linear.z = z * speed;

    twist.angular.x = 0;
    twist.angular.y = 0;
    twist.angular.z = th * turn;

    head_Tws.linear.x = xb * 1 ; //lifter
    head_Tws.linear.y = yb * 1 ; //gripper

    state_.data = true;
    imu_override_.data = true;
    leg_height_.data = true;
  
    ROS_INFO("%d, %d, %d, %d, %d,", batas[0], batas[1], batas[2], batas[3], batas[4]);
    ROS_INFO("%d, %d, %d, %d, %d,",ping[0],ping[1],ping[2],ping[3],ping[4]);
    ROS_INFO("%d, %d, %d, %d, %d, ",flag_[0],flag_[1],flag_[2],flag_[3],flag_[4]);


    bool s[5]={false,false,false,false,false};

  if(pilih==true){
    for (int a=0; a<5; a++){
      if(flag_[a]==true){
        if(ping[a]<=batas[a])
        {
          s[a]=true;
        }
        else{s[a]=false;}
      }
      else{
        if(ping[a]>=batas[a])
        {
          s[a]=true;
        }
        else{s[a]=false;}
      }
      yaa[a]=xaa[a];
    }
  }

  else{

    for (int a=0; a<5; a++){
      xas[a]=xaa[a]-yaa[a];
      if(flag_[a]==true){
        if(xas[a]<=batas[a])
        {
          s[a]=true;
        }
        else{s[a]=false;}
      }
      else{
        if(xas[a]>=batas[a])
        {
          s[a]=true;
        }
        else{s[a]=false;}
      }
    }
  }

// //   // ROS_INFO("%d, %d, %d, %d, %d, %d, %d, %d, ",s[0], s[1], s[2], s[3], s[4]);
  
//   if(s[0]==true && s[1]==true && s[2]==true && s[3]==true && s[4]==true){
//     flag1++;
//     ROS_INFO("clear");
//     yaa[0]=xaa[0];
//     yaa[1]=xaa[1];
//     yaa[2]=xaa[2];
//   }
}

 
int main(int argc, char **argv)
{
   flag1=0;
  ros::init(argc, argv, "Move_Control");
  ros::NodeHandle n;
  ros::Publisher pub = n.advertise<geometry_msgs::Twist>("/cmd_vel", 1);
  ros::Publisher head_pub_ = n.advertise<geometry_msgs::Twist>("/head_Tws", 1);
  ros::Publisher imu_override_pub_ = n.advertise<std_msgs::Bool>("/imu/imu_override", 100);
  ros::Publisher leg_height_pub_ = n.advertise<std_msgs::Bool>("/leg", 100);
  ros::Publisher state_pub_ = n.advertise<std_msgs::Bool>("/state", 100);
  ros::Subscriber sub = n.subscribe("merged_ping_topic", 10, mergedPingCallback);

  // flag1=1;
  ros::Rate r(100); 
  while (ros::ok())
  {
    // //baca setpoin
    //  ROS_INFO("-------------------------");
    //  ROS_INFO("%f, %f, %f, %f, %f",xas[0],xas[1],xas[2],xas[3],xas[4]);
    // //  ROS_INFO("I heard: [%d] [%d]", ir, pb);
    // for(int i = 0; i < 5; i++) {
    //   ROS_INFO(": [%i]", ping[i]);
    // }
    
    //eksekusi
      kontrol(a_gerak[flag1],flag1);
      
      state_pub_.publish(state_);
      pub.publish(twist);
      imu_override_pub_.publish(imu_override_);
      leg_height_pub_.publish(leg_height_);
      head_pub_.publish(head_Tws);
      // qwerty.data=b_gerak[flag1];
      // pub_f_servo.publish(qwerty);

      // std_msgs::UInt16 asd;
      // asd.data=gerak_1_[flag1];
      // pub_pompa.publish(asd);

      // ROS_INFO("step: %s", qwerty.data);
      // ROS_INFO("step: %d, %d", flag1,gerak_1_[flag1] );


    ros::spinOnce();
    r.sleep();
  }
  return 0;
}