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

float ping[6]={0,0,0,0,0,0};
// kanan, depan, belakang, kiri, imu z, imu w
void tofdistancesCallback(const std_msgs::Int32MultiArray::ConstPtr& msg)
{
  for (int i=0;i<4;i++){
    ping[i]=msg->data[i];
  }
  ROS_INFO("I heard: [%f]", ping[0]);
}
void imuCallback(const sensor_msgs::Imu::ConstPtr& msg)
{
  ping[4] = msg->orientation.z;
  ping[5] = msg->orientation.y;

  ROS_INFO("IMU Orientation: z=%f, y=%f", ping[4], ping[5]);
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
char a_gerak[]  ={'d','x','A','w','s','s','x','A','A','x','D','a','x','w','A','w','d','A','w','A','D','d','w','s','s','x','a','w','A','x','w','d','A','w','A','w','D','w','D','w','D','a','A','w','D','w','D','a','w','s','s','x','A','d','D','D','w','w','a','w','s'}; 
// step            0   1   2   3   4   5   6   7   8   9  10   11  12  13  14  15  16  17  18  19  20  21  22  23 24   25  26  27  28  29  30  31  32  33  34  35  36 37  38   39  40  41  42 43  44   45  46  47  48  49  50  51  52  53
int gerak_1_[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

//program buat limit sensor dan gerakan kaki dan juga gerakan gripper
std::map<int, std::vector<float>> step{
  // {1, {0,0,-2,0,0,0,0,0,0.5,0.5}},   //batas 0-7, speed, turn  //rotate kanan
  //{step, {Tof_Kanan, Tof_depan, Tof_belakang, Tof_Kiri, Imu Z, Imu X, Gripper, Gripper}}
  {0, {200,0,0,500,0.63,-1,-2,0,1,1}},
  {1, {0,0,60,0,-1,-1,-2,0,1,1}},
  {2, {520,250,0,0,-1,-1,-2,0,2,1}}, //keluar home
  {3, {0,0,190,0,-1,-1,0,-1,1,1}}, //ke k1
  {4, {0,100,0,0,-1,-1,0,0,1,1}}, //k1
  {5, {0,100,0,0,-1,-1,-2,0,1,1}},
  {6, {0,0,80,0,-1,-1,-2,0,1,1}}, //dari k1
  {7, {624,0,0,0,-1,-1,-2,0,2,1}},
  {8, {0,500,0,190,-1,-1,-2,0,1,1}}, //ke jalan retak, miring, batu
  {9, {0,0,50,0,-1,-1,-2,0,1,1}},
  {10, {0,0,0,200,-1,-1,-2,0,1,1}},
  {11, {0,240,480,100,-1,-1,-2,0,1,2}}, //ke sz1
  {12, {0,200,0,0,-1,-1,-2,0,1,1}}, //ke sz1
  {13, {0,200,0,0,-1,-1,-2,0,1,1}},
  {14, {0,0,0,60,-1,-1,-2,0,1,1}},
  {15, {0,150,0,0,-1,-1,0,-1,1,1}}, //sz1
  {16, {400,400,250,200,0.64,-1,-2,0,1,2}}, //arah kelereng
  {17, {0,0,0,100,-1,-1,-2,0,1,1}},
  {18, {0,459,517,0,-1,-1,-2,0,1,1}}, //ke k2
  {19, {0,0,0,70,-1,-1,-2,0,1,1}},
  {20, {0,0,0,70,-1,-1,-2,0,1,1}},
  {21, {0,260,0,280,-0.00,-1,-2,0,1,2}}, //hadap k2
  {22, {0,110,180,0,-1,-1,0,-1,1,1}}, //ke k2
  {23, {0,110,0,0,-1,-1,0,0,1,1}}, //k2
  {24, {0,120,0,0,-1,-1,-2,0,1,1}},
  {25, {0,0,70,0,-1,-1,-2,0,1,1}},
  {26, {0,0,550,50,0.63,-1,-2,0,1,2}}, //arah sz2 //d
  {27, {0,180,0,0,-1,-1,-2,0,1,1}}, //ke sz2 //w
  {28, {0,0,0,50,-1,-1,-2,0,1,1}}, 
  {29, {0,200,0,0,-1,-1,-2,0,1,1}},
  {30, {0,300,0,0,-1,-1,0,-1,1,1}}, //sz2 //s
  {31, {0,400,200,200,-0.1,-1,-2,0,1,2}}, //keluar sz2 //a
  {32, {0,0,0,70,-1,-1,-2,0,2,1}},
  {33, {200,0,750,0,-1,-1,-2,0,1,1}}, //menuju jalan retak
  {34, {0,0,0,100,-1,-1,-2,0,1,1}},
  {35, {0,0,790,0,-1,-1,-2,0,1,1}},
  {36, {120,0,0,370,-1,-1,-2,0,2,1}}, //menuju jalan retak
  {37, {0,300,0,0,-1,-1,0.1,0,1,1}},
  {38, {120,0,0,0,-1,-1,-2,0,1,1}},
  {39, {0,180,300,250,-1,0.1,-2,0,1,1}}, //melewati jalan retak dan batu
  {40, {160,0,0,0,-1,-1,-2,0,1,1}},
  {41, {150,300,180,400,0.62,-1,-2,0,1,1}}, //posisi siap naik tangga
  {42, {150,0,0,0,-1,-1,-2,0,1,1}},
  {43, {0,140,0,0,-1,-1,-2,0,1,1}}, //posisi siap naik tangga
  {44, {0,0,550,0,-1,-1,-2,0,1,1}}, //naik tangga
  {45, {0,200,0,0,-1,-1,-2,0,1,1}},
  {46, {70,0,200,0,-1,-1,-2,0,1,1}},
  // 'd','w','s','s','x','D','a','A','x','w','A','w','s','d','w','s'
  // 45                                                           58
  {47, {0,0,0,600,0.62,-1,-2,0,1,1}},
  {48, {0,120,190,0,-1,-1,0,-1,1,1}},
  {49, {0,120,0,0,0,-1,0,0,1,1}}, //hadap k5
  {50, {0,100,0,0,-1,-1,-2,0,1,1}}, //hadap k5
  {51, {0,0,100,0,-1,-1,-2,0,1,1}}, //di k5
  {52, {340,0,400,0,-1,-1,-2,0,1,1}}, //di k5
  {53, {0,300,0,220,-0.00,-1,-2,0,1,1}},
  {54, {0,0,0,370,-1,-1,-2,0,1,1}}, //dari k5
  {55, {137,0,300,0,-1,-1,-2,0,1,1}}, //sz5
  {56, {0,0,250,0,-1,-1,-2,0,1,1}},
  {57, {0,200,0,0,-1,-1,0,-1,1,1}},
  {58, {400,200,0,200,-1,-1,-2,0,1,1}}, //arah finish
  {59, {0,180,0,0,-1,-1,-2,0,1,1}}, //finish
  {60, {0,0,0,0,-1,-1,0,-1,1,1}},
};

std::map<int, std::vector<bool>> _f_{
  // ini program untuk kondisi if 1 atau 0 (komparasi)

  // {1, {0,0,1,0,0,0,0,0,0}},  //kompar 0-6 (0)(sensor>=batas) (1)(Sensor<=batas), LaserOrOdom(1=laser && 0=odom) //odom ,imu over , leg height
  //uneven = 0,1 && normal = 0,0 ( 2 digit terakhir) 
  {0, {1,0,0,0,1,0,1,0,0}},
  {1, {0,0,1,0,0,0,1,0,0}},
  {2, {0,0,0,0,0,0,1,0,0}}, // posisi home gerak ke kanan semua sensor nilai lebih dari batas
  {3, {0,0,0,0,0,0,1,0,0}},
  {4, {0,1,0,0,0,0,1,0,0}},
  {5, {0,0,0,0,0,0,1,0,0}},
  {6, {0,0,1,0,0,0,1,0,0}},
  {7, {0,0,0,0,0,0,1,0,0}},
  {8, {0,0,0,1,0,0,1,0,1}},
  {9, {0,0,1,0,0,0,1,0,1}},
  {10, {0,0,0,0,0,0,1,0,1}},
  {11, {0,1,0,1,0,0,1,0,1}},
  {12, {0,0,0,0,0,0,1,0,1}},
  {13, {0,1,0,0,0,0,1,0,1}},
  {14, {0,0,0,1,0,0,1,0,1}},
  {15, {0,1,0,0,0,0,1,0,1}},
  {16, {1,0,1,0,1,0,1,0,1}},
  {17, {0,0,0,1,0,0,1,0,1}},
  {18, {0,1,0,0,0,0,1,0,1}},
  {19, {0,0,0,1,0,0,1,0,1}},
  {20, {0,0,0,0,0,0,1,0,1}},
  {21, {0,1,0,0,1,0,1,0,1}},
  {22, {0,1,0,0,0,0,1,0,1}},
  {23, {0,1,0,0,0,0,1,0,1}},
  {24, {0,0,0,0,0,0,1,0,1}},
  {25, {0,0,1,0,0,0,1,0,1}},
  {26, {0,0,0,1,0,0,1,0,1}},
  {27, {0,1,0,0,0,0,1,0,1}},
  {28, {0,0,0,1,0,0,1,0,1}},
  {29, {0,0,0,0,0,0,1,0,1}},
  {30, {0,1,0,0,0,0,1,0,1}},
  {31, {0,0,1,0,1,0,1,0,1}},
  {32, {0,0,0,1,0,0,1,0,1}},
  {33, {0,0,0,0,0,0,1,0,1}},
  {34, {0,0,0,1,0,0,1,0,1}},
  {35, {0,0,0,0,0,0,1,0,1}},
  {36, {1,0,0,0,0,0,1,0,0}},
  {37, {0,1,0,0,0,0,1,0,1}},
  {38, {1,0,0,0,0,0,1,0,1}},
  {39, {0,1,0,0,0,0,1,0,1}},
  {40, {1,0,0,0,0,0,1,0,1}},
  {41, {1,0,1,0,0,0,1,0,1}},
  {42, {0,0,0,0,0,0,1,0,1}},
  {43, {0,1,0,0,0,0,1,0,1}},
  {44, {0,0,0,0,0,0,1,0,1}},
  {45, {0,1,0,0,0,0,1,0,1}},
  {46, {1,0,1,0,0,0,1,0,1}},
  //'d','w','s','s','x','D','a','A','x','w','A','w','s','d','w','s'
  {47, {0,0,0,0,0,0,1,0,1}},
  {48, {0,1,0,0,0,0,1,0,1}},
  {49, {0,1,0,0,0,0,1,0,1}},
  {50, {0,0,0,0,0,0,1,0,1}},
  {51, {0,0,1,0,0,0,1,0,1}},
  {52, {0,0,0,0,0,0,1,0,1}},
  {53, {0,0,0,1,1,0,1,0,1}},
  {54, {0,0,0,0,0,0,1,0,1}},
  {55, {1,0,1,0,0,0,1,0,1}},
  {56, {0,0,0,0,0,0,1,0,1}},
  {57, {0,1,0,0,0,0,1,0,1}},
  {58, {0,1,0,1,0,0,1,0,1}},
  {59, {0,1,0,0,0,0,1,0,1}},
  {60, {1,1,1,1,0,0,1,0,1}},
};

// Init variables
float speed(1);                                                 // Linear velocity (m/s)
float turn(1);                                                  // Angular velocity (rad/s)
float x(0), y(0), z(0), xa(0), ya(0), za(0), xb(0), yb(0), th(0); // Forward/backward/neutral direction vars
char key(' ');
int offset(15);
bool isAvoidanceActive = false;
int currentStep = 0;
geometry_msgs::Twist twist;
geometry_msgs::Twist head_Tws;
std_msgs::Bool imu_override_;
std_msgs::Bool leg_height_;
std_msgs::Bool state_;
std_msgs::Int32 Led_;

void avoidance(){
  
  if (ping[0] <= 30 || ping[1] <=90 || ping[2] <= 40 || ping[3] <= 30) {
    isAvoidanceActive = true;
    if(ping[0] <= 30){
    //gerakan ke kiri
    twist.linear.x = 0;
    twist.linear.y = -0.5;
    twist.linear.z = 0;
    twist.angular.x = 0;
    twist.angular.y = 0;
    twist.angular.z = 0;
 
  }
  if(ping[1] <= 70 && xb == -2){
    //gerakan mundur
    twist.linear.x = -0.5;
    twist.linear.y = 0;
    twist.linear.z = 0;
    twist.angular.x = 0;
    twist.angular.y = 0;
    twist.angular.z = 0;

  }
  if(ping[1] <= 70 && xb == 0 && yb == 0){
    //gerakan mundur
    twist.linear.x = -0.5;
    twist.linear.y = 0;
    twist.linear.z = 0;
    twist.angular.x = 0;
    twist.angular.y = 0;
    twist.angular.z = 0;

  }
    if(ping[2] <= 40){
    //gerakan maju
    twist.linear.x = 0.2;
    twist.linear.y = 0;
    twist.linear.z = 0;
    twist.angular.x = 0;
    twist.angular.y = 0;
    twist.angular.z = 0;

  }
  if(ping[3] <= 30){
    //gerakan ke kanan
    twist.linear.x = 0;
    twist.linear.y = 0.5;
    twist.linear.z = 0;
    twist.angular.x = 0;
    twist.angular.y = 0;
    twist.angular.z = 0;

  }
  } else {
    isAvoidanceActive = false;
  }
}

bool pilih;
void kontrol(char arah_, int step_){
  if (isAvoidanceActive) {
    return;
  }
  key=arah_;
  int batas[6];
  if (step.count(step_) == 1)
    {
      for(int a=0;a<6;a++){
        batas[a]=step[step_][a];
      }
      // currentStep = step_;
      xb=step[step_][6];
      yb=step[step_][7];
      speed=step[step_][8];
      turn=step[step_][9];
    }

  bool flag_[6];
  if (_f_.count(step_) == 1)
    {
      for(int a=0;a<6;a++){
        flag_[a]=_f_[step_][a];
      }
    pilih=_f_[step_][6];
    imu_override_.data = _f_[step_][7];
    leg_height_.data = _f_[step_][8];
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
    twist.linear.x = x * speed *0.5;
    twist.linear.y = y * speed* 0.5;
    twist.linear.z = z * speed *0.5;

    twist.angular.x = 0;
    twist.angular.y = 0;
    twist.angular.z = th * turn *0.5;

    head_Tws.linear.x = xb * 0.625 ; //lifter
    head_Tws.linear.y = yb * 1.1 ; //gripper

    state_.data = true;
    Led_.data=2;
    
  
    ROS_INFO("%d, %d, %d, %d ", batas[0], batas[1], batas[2], batas[3]);
    ROS_INFO("%f, %f, %f, %f, %f, %f",ping[0],ping[1],ping[2],ping[3],ping[4],ping[5]);
    ROS_INFO("%d, %d, %d, %d",flag_[0],flag_[1],flag_[2],flag_[3]);


    bool s[6]={false,false,false,false,false,false};

  if(pilih==true){
    for (int a=0; a<6; a++){
    for (int a = 0; a < 6; a++) {
        if (flag_[a] == true) {
            if (ping[a] <= (batas[a] + offset) || ping[a] <= (batas[a] - offset)) {
                s[a] = true;
            } else {
                s[a] = false;
            }
        } else {
            if (ping[a] >= (batas[a] + offset) || ping[a] >= (batas[a] - offset)) {
                s[a] = true;
            } else {
                s[a] = false;
            }
        }
        yaa[a] = xaa[a];
    }
  }
}
  else{

    for (int a=0; a<6; a++){
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
  
  if(s[0]==true && s[1]==true && s[2]==true && s[3]==true && s[4]==true && s[5]==true){
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
  ros::Subscriber imu_sub = n.subscribe("/imu/data", 10, imuCallback);
  ros::Subscriber sub = n.subscribe("tof_distances", 10, tofdistancesCallback);

  ros::Subscriber _sub1 = n.subscribe("/chatter1", 1, chatter1Callback);
  ros::Subscriber _sub2 = n.subscribe("/chatter2", 1, chatter2Callback);
  ros::Subscriber _sub3 = n.subscribe("/chatter3", 1, chatter3Callback);

  // flag1=1;
  ros::Rate r(100); 
  while (ros::ok())
  {
    // //baca setpoin
    // ROS_INFO("-------------------------");
     //ROS_INFO("%f, %f, %f, %f, %f",xas[0],xas[1],xas[2],xas[3],xas[4]);
    // //  ROS_INFO("I heard: [%d] [%d]", ir, pb);
    // for(int i = 0; i < 5; i++) {
    //   ROS_INFO(": [%i]", ping[i]);
    // }
    
    //eksekusi
      avoidance();
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
