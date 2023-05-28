#include "ros/ros.h"
#include "std_msgs/String.h"

#include <nav_msgs/Odometry.h>
#include <std_msgs/Int32MultiArray.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/AccelStamped.h>
#include <std_msgs/Bool.h>
#include <std_msgs/UInt16.h>
#include <std_msgs/Float32.h>
#include <std_msgs/String.h>
#include <std_msgs/Int32.h>
#include <sensor_msgs/Imu.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <map>

int ping[4]={0,0,0,0};
// Depan kanan, Belakang Kanan, Belakang, Belakang kiri, depan kiri
void tofdistancesCallback(const std_msgs::Int32MultiArray::ConstPtr& msg)
{
  for (int i=0;i<4;i++){
    ping[i]=msg->data[i];
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
char a_gerak[]  ={'D', 'w', 's', 'x', 'D','d', 's', 'a', 'w', 'a', 'w', 's', 'd', 'w', 's', 'a', 'w', 'a','w','d','w', 'x', 'A', 'a','d', 'D','x','w','x', 'A','w','A','a','w','d','w','c','A','w','s'};

int gerak_1_[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

//program buat limit sensor dan gerakan kaki dan juga gerakan gripper
std::map<int, std::vector<int>> step{
  // {1, {0,0,-2,0,0,0,0,0,0.5,0.5}},   //batas 0-7, speed, turn  //rotate kanan
  // Penejlasan {urutan gerakan , {lmit sensor 1,2,3,4,5 , nilai gripper x , nilai gripper y}}
  // Depan kanan, Belakang Kanan, Belakang, Belakang kiri, depan kiri,lifter , gripper
  //{step, {Tof_Kanan, Tof_depan, Tof_belakang, Tof_Kiri, Gripper, Gripper}}
  {0, {0, 250, 0, 550, -2 ,0}},
  {1, {0, 110, 220, 0, 0, -1}},
  {2, {0, 250, 0, 0, -2, 0}},
  {3, {0, 0, 120, 0, -2, 0}},
  {4, {325, 800, 0, 350, -2, 0}},
  {5, {180, 260, 240, 190, -2, 0}},
  {6, {0, 120, 0, 0, 0, -1}},
  {7, {0, 840, 145, 0, -2, 0}},
  {8, {0, 0, 383, 0, -2, 0}},
  {9, {320,118,250,430,0,-1}},
  {10, {0,180,0,0,-2,0}},
  {11, {242,473,423,178,-2,0}},
  {12, {0,248,0,0,-2,0}},
  {13, {0,125,0,0,0,-1}},
  {14, {360,629,195,260,-2,0}},
  {15, {205,285,370,450,-2,0}},
  {16, {234,0,275,0,-2,0}},
  {17, {0,397,368,0,-2,0}},
  {18, {0,295,166,0,-2,0}},
  {19, {0,115,190,0,0,-1}},
  {20, {0,255,168,0,0,-1}},
  {21, {160,0,0,425,-2,0}},
  {22, {450,260,352,385,-2,0}},
  {23, {630, 270,230,300,0,-1}},
  {24, {340, 90, 0, 710,-2,0}},
  {25, {0,0,160,0,-2,0}},
  {26, {0,0,170,0,0,-1}},
  {27, {0,0,150,0,-2,0}},
  {28, {290,0,0,150,-2,0}},
  {29, {0,0,307,0,-2,0}},
  {30, {0,0,770,390,-2,0}},
  {31, {220,470,0,750,-2,0}},
  {32, {0,230,0,0,-2,0}},
  {33, {235,260,0,0,0,-1}},
  {34, {0,220,0,0,0,-1}},
  {35, {180,280,0,550,-2,0}},
  {36, {660,0,0,190,-2,0}},
  {37, {0,210,315,0,-2,0}},
  {38, {0 ,0 ,0 ,0 ,-2,0}},
  {39, {185,180,325,770,-2,0}}

};

std::map<int, std::vector<bool>> _f_{
  // ini program untuk kondisi if 1 atau 0 (komparasi)
  // {1, {0,0,1,0,0,0,0,0,0}},  //kompar 0-4 (0)(sensor>=batas) (1)(Sensor<=batas), LaserOrOdom(1=lase && 0=odom) //odom ,imu over , leg height
  //uneven = 0,1 && normal = 0,0 ( 2 digit terakhir) 
  {0, {0,0,0,0,1,0,1}}, // posisi home gerak ke kanan semua sensor nilai lebih dari batas
  {1, {0,1,0,0,1,0,1}},
  {2, {0,1,0,0,1,0,1}},
  {3, {0,0,1,0,1,0,1}},
  {4, {1,0,0,0,1,0,1}},
  {5, {0,0,0,0,1,0,1}},
  {6, {0,1,0,0,1,0,1}},
  {7, {0,0,0,0,1,0,1}},
  {8, {0,0,0,0,1,0,1}},
  {9, {0,1,1,0,1,0,1}},
  {10, {0,0,0,0,1,0,1}},
  {11, {1,0,0,1,1,0,1}},
  {12, {0,1,0,0,1,0,1}},
  {13, {0,1,0,0,1,0,1}},
  {14, {1,0,1,1,1,0,1}},
  {15, {1,1,0,0,1,0,1}},
  {16, {0,0,1,0,1,0,1}},
  {17, {0,1,0,1,1,0,1}},
  {18, {0,1,1,0,1,0,1}},
  {19, {0,1,0,0,1,0,1}},
  {20, {0,0,1,0,1,0,1}},
  {21, {0,0,0,1,1,0,1}},
  {22, {0,1,0,1,1,0,1}},
  {23, {0,0,0,0,1,0,1}},
  {24, {1,1,0,0,1,0,1}},
  {25, {0,0,1,0,1,0,1}},
  {26, {0,0,0,0,1,0,1}},
  {27, {0,0,1,0,1,0,1}},
  {28, {0,0,0,1,1,0,1}},
  {29, {0,0,0,0,1,0,1}},
  {30, {0,0,0,1,1,0,1}},
  {31, {1,0,0,0,1,0,1}},
  {32, {0,1,0,0,1,0,1}},
  {33, {0,1,0,0,1,0,1}},
  {34, {0,1,0,0,1,0,1}},
  {35, {1,0,0,0,1,0,1}},
  {36, {0,0,0,1,1,0,1}},
  {37, {0,1,0,0,1,0,1}},
  {38, {0,0,0,0,1,0,1}},
  {39, {1,1,0,0,1,0,1}},

  // {0, {0,0,0,0,1,0,1}}
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
std_msgs::Int32 Led_;



bool pilih;
void kontrol(char arah_, int step_){
  key=arah_;
  int batas[4];
  if (step.count(step_) == 1)
    {
      for(int a=0;a<4;a++){
        batas[a]=step[step_][a];
      }
      xb=step[step_][4];
      yb=step[step_][5];
    }

  bool flag_[4];
  if (_f_.count(step_) == 1)
    {
      for(int a=0;a<4;a++){
        flag_[a]=_f_[step_][a];
      }
    pilih=_f_[step_][4];
    imu_override_.data = _f_[step_][5];
    leg_height_.data = _f_[step_][6];
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
    Led_.data=2;
    
  
    ROS_INFO("%d, %d, %d, %d ", batas[0], batas[1], batas[2], batas[3]);
    ROS_INFO("%d, %d, %d, %d",ping[0],ping[1],ping[2],ping[3]);
    ROS_INFO("%d, %d, %d, %d",flag_[0],flag_[1],flag_[2],flag_[3]);


    bool s[4]={false,false,false,false};

  if(pilih==true){
    for (int a=0; a<4; a++){
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

    for (int a=0; a<4; a++){
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
//  //ROS_INFO("%d, %d, %d, %d ",s[0], s[1], s[2], s[3], s[4]);
  
  if(s[0]==true && s[1]==true && s[2]==true && s[3]==true ){
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
  ros::param::get("TELEOP_SPEED", speed);
  ros::param::get("TELEOP_SPEED", turn);
  ros::Publisher pub = n.advertise<geometry_msgs::Twist>("/cmd_vel", 1);
  ros::Publisher head_pub_ = n.advertise<geometry_msgs::Twist>("/head_Tws", 1);
  ros::Publisher imu_override_pub_ = n.advertise<std_msgs::Bool>("/imu/imu_override", 100);
  ros::Publisher leg_height_pub_ = n.advertise<std_msgs::Bool>("/leg", 100);
  ros::Publisher state_pub_ = n.advertise<std_msgs::Bool>("/state", 100);
  ros::Publisher Led = n.advertise<std_msgs::Int32>("/led_control", 10);
  ros::Subscriber sub = n.subscribe("tof_distances", 10, tofdistancesCallback);

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
      Led.publish(Led_);
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