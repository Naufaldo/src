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
void chatter1Callback(const std_msgs::Float32& msg)
{
  xaa[0]=msg.data;
  // ROS_INFO("I heard: [%f]", xaa[0]);
  if(ff1==false){ yaa[0]=xaa[0]; ff1=true;}
}

void chatter2Callback(const std_msgs::Float32& msg)
{
  xaa[1]=msg.data;
  // ROS_INFO("I heard: [%f]", xaa[1]);
  if(ff2==false){ yaa[1]=xaa[1];ff2=true;}
}

void chatter3Callback(const std_msgs::Float32& msg)
{
  xaa[2]=msg.data;
  // ROS_INFO("I heard: [%f]", xaa[2]);
  if(ff3==false){ yaa[2]=xaa[2];ff3=true;}
}

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

//step
char a_gerak[]  ={'a','w','x','a','x','a','s','d'};

int gerak_1_[]={0,0,0,0,0,0,0,0,0};


//program buat limit sensor dan gerakan kaki dan juga gerakan gripper
std::map<int, std::vector<int>> step{
  // {1, {0,0,-2,0,0,0,0,0,0.5,0.5}},   //batas 0-7, speed, turn  //rotate kanan
  // Penejlasan {urutan gerakan , {lmit sensor 1,2,3,4,5 , nilai gripper x , nilai gripper y}}
  {0, {10,10,7,0,320,-2 ,0}},
  {1, {15,15,0,7,7,-2 ,0}}, // posisi home gerak ke kanan
  {2, {120,320,45,5,5,0,0}},
  {3, {46,320,21,59,60,0,0}},
  {4, {52,320,7,18,18,0,0}},
  // {4, {52,320,7,18,18},0},
  // {5, {52,320,7,18,18},0},
  // {6, {52,320,7,18,18},0},
  // {7, {52,320,7,18,18},0},
  

  
};
std::map<int, std::vector<bool>> _f_{
  // ini program untuk kondisi if 1 atau 0 (komparasi)
  // {1, {0,0,1,0,0,0,0,0,0}},  //kompar 0-4 (0)(sensor>=batas) (1)(Sensor<=batas), LaserOrOdom(1=lase && 0=odom) //odom ,imu over , leg height
  //uneven = 0,1 && normal = 0,0 ( 2 digit terakhir) 
  {0, {0,0,0,0,1,1,0,1}}, // posisi home gerak ke kanan semua sensor nilai lebih dari batas
  {1, {1,1,0,0,0,1,0,0}},
  {2, {1,1,0,0,0,1,0,0}},
  {3, {0,0,0,0,0,1,0,1}},

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
    imu_override_.data = _f_[step_][6];
    leg_height_.data = _f_[step_][7];
    }
    

  if (moveBindings.count(key) == 1)
    {
      // Grab the direction data
      x = moveBindings[key][0];
      y = moveBindings[key][1];
      z = moveBindings[key][2];
      th = moveBindings[key][3];
      imu_override_.data = false;
           
      ROS_INFO("\rCurrent: speed %f   | turn %f | Last command: %c   ", speed, turn, key);
    }

    // Update the Twist message
    twist.linear.x = x * speed;
    twist.linear.y = y * speed;
    twist.linear.z = z * speed;

    twist.angular.x = 0;
    twist.angular.y = 0;
    twist.angular.z = th * turn;

    head_Tws.linear.x = xb * 0.7 ; //lifter
    head_Tws.linear.y = yb * turn ; //gripper

    state_.data = true;
    
  
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

 ROS_INFO("%d, %d, %d, %d, %d, %d, %d, %d, ",s[0], s[1], s[2], s[3], s[4]);
  
  if(s[0]==true && s[1]==true && s[2]==true && s[3]==true && s[4]==true){
    flag1++;
    ROS_INFO("clear");
    yaa[0]=xaa[0];
    yaa[1]=xaa[1];
    yaa[2]=xaa[2];
  }
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

  ros::Subscriber _sub1 = n.subscribe("/chatter1", 1, chatter1Callback);
  ros::Subscriber _sub2 = n.subscribe("/chatter2", 1, chatter2Callback);
  ros::Subscriber _sub3 = n.subscribe("/chatter3", 1, chatter3Callback);

  // flag1=1;
  ros::Rate r(100); 
  while (ros::ok())
  {
    // //baca setpoin
     //ROS_INFO("-------------------------");
     //ROS_INFO("%f, %f, %f, %f, %f",xas[0],xas[1],xas[2],xas[3],xas[4]);
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
      ROS_INFO("step: %d, %d", flag1,gerak_1_[flag1] );


    ros::spinOnce();
    r.sleep();
  }
  return 0;
}