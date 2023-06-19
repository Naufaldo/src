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
  ping[4] = msg->orientation.w;
  ping[5] = msg->orientation.z;

  ROS_INFO("IMU Orientation: w=%f, z=%f", ping[4], ping[5]);
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
char a_gerak[]  ={'x','A','w','s','s','x','A','x','D','a','x','w','A','s','d','A','w','A','D','d','w','s','s','x','a','w','A','s','d','A','w','D','w','D','a','A','w','D','w','D','A','w','d','A','w','s','s','x','D','D','w','s','a','w','s'}; 
// step            0   1   2   3   4   5   6   7   8   9  10   11  12  13  14  15  16  17  18  19  20  21  22  23 24   25  26  27  28  29  30  31  32  33  34  35  36 37  38   39  40  41  42 43  44   45  46  47  48  49  50  51 52  53  54
int gerak_1_[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

//program buat limit sensor dan gerakan kaki dan juga gerakan gripper
std::map<int, std::vector<float>> step{
  // {1, {0,0,-2,0,0,0,0,0,0.5,0.5}},   //batas 0-7, speed, turn  //rotate kanan
  //{step, {Tof_Kanan, Tof_depan, Tof_belakang, Tof_Kiri, Imu Z, Imu X, Gripper, Gripper}}
  {0, {0,0,60,0,-1,-1,-2,0,1,1}},
  {1, {480,290,0,0,-1,-1,-2,0,1,1}}, //keluar home
  {2, {0,0,190,0,-1,-1,0,-1,1,1}}, //ke k1
  {3, {0,140,0,0,-1,-1,0,0,1,1}}, //k1
  {4, {0,140,0,0,-1,-1,-2,0,1,1}},
  {5, {0,0,80,0,-1,-1,-2,0,2,1}}, //dari k1
  {6, {0,500,190,0,-1,-1,-2,0,1,1}}, //ke jalan retak, miring, batu
  {7, {0,0,50,0,-1,-1,-2,0,1,1}},
  {8, {0,0,0,200,-1,-1,-2,0,1,1}},
  {9, {0,240,480,100,-1,-1,-2,0,1,1}}, //ke sz1
  {10, {0,180,0,0,-1,-1,-2,0,1,1}}, //ke sz1
  {11, {0,200,0,0,-1,-1,-2,0,1,1}},
  {12, {0,0,0,90,-1,-1,-2,0,1,1}},
  {13, {0,170,0,0,-1,-1,0,-1,1,1}}, //sz1
  {14, {400,400,280,200,0.78,-1,-2,0,1,1}}, //arah kelereng
  {15, {0,0,0,200,-1,-1,-2,0,1,1}},
  {16, {0,459,517,0,-1,-1,-2,0,1,1}}, //ke k2
  {17, {0,0,0,70,-1,-1,-2,0,1,1}},
  {18, {0,0,0,70,-1,-1,-2,0,1,1}},
  {19, {0,260,0,280,0.0,-1,-2,0,1,2}}, //hadap k2
  {20, {0,100,0,0,-1,-1,0,-1,1,1}}, //ke k2
  {21, {0,100,0,0,-1,-1,0,0,1,1}}, //k2
  {22, {0,120,0,0,-1,-1,-2,0,1,1}},
  {23, {0,0,70,0,-1,-1,-2,0,1,1}},
  {24, {0,0,550,50,0.80,-1,-2,0,1,1}}, //arah sz2 //d
  {25, {0,180,0,0,-1,-1,-2,0,1,1}}, //ke sz2 //w
  {26, {0,0,0,50,-1,-1,-2,0,1,1}}, 
  {27, {0,150,0,0,-1,-1,0,-1,1,1}}, //sz2 //s
  {28, {0,400,200,200,-0.960,0,-2,0,1,1}}, //keluar sz2 //a
  {29, {0,0,0,70,-1,-1,-2,0,1,1}},
  {30, {0,0,750,0,-1,-1,-2,0,1,1}}, //menuju jalan retak
  {31, {120,0,0,370,-1,-1,-2,0,2,1}}, //menuju jalan retak
  {32, {0,180,0,250,-1,-1,-2,0,1,1}}, //melewati jalan retak dan batu
  {33, {190,0,0,0,-1,-1,-2,0,1,1}},
  {34, {150,300,80,500,0.80,-1,-2,0,1,1}}, //posisi siap naik tangga
  {35, {200,0,0,0,-1,-1,-2,0,1,1}},
  {36, {0,140,0,0,-1,-1,-2,0,1,1}}, //posisi siap naik tangga
  {37, {220,0,600,0,-1,-1,-2,0,1,1}}, //naik tangga
  {38, {0,0,120,0,-1,-1,-2,0,1,1}},
  {39, {120,0,100,0,-1,-1,-2,0,1,1}},
  {40, {0,0,0,220,-1,-1,-2,0,1,1}},
  {41, {0,100,0,0,-1,-1,-2,0,1,1}},
  {42, {400,300,0,180,-1,-1,-2,0,1,1}}, //hadap k5
  {43, {0,0,0,140,-1,-1,-2,0,1,1}}, //hadap k5
  {44, {0,100,0,0,-1,-1,0,-1,1,1}}, //di k5
  {45, {0,100,0,0,-1,-1,0,0,1,1}}, //di k5
  {46, {0,120,0,0,-1,-1,0,0,1,1}},
  {47, {400,300,0,0,-1,-1,-2,0,1,1}}, //dari k5
  {48, {500,700,700,300,-1,-1,-2,0,1,1}},
  {49, {150,400,0,0,-1,-1,-2,0,1,1}}, //ke sz5
  {50, {300,0,0,0,-1,-1,-2,0,1,1}}, //sz5
  {51, {0,200,0,0,-1,-1,0,-1,1,1}},
  {52, {400,200,0,200,-1,-1,-2,0,1,1}}, //arah finish
  {53, {0,100,0,0,-1,-1,-2,0,1,1}}, //finish
  {54, {0,0,0,0,-1,-1,0,-1,1,1}},
};

std::map<int, std::vector<bool>> _f_{
  // ini program untuk kondisi if 1 atau 0 (komparasi)

  // {1, {0,0,1,0,0,0,0,0,0}},  //kompar 0-6 (0)(sensor>=batas) (1)(Sensor<=batas), LaserOrOdom(1=laser && 0=odom) //odom ,imu over , leg height
  //uneven = 0,1 && normal = 0,0 ( 2 digit terakhir) 
  {0, {0,0,1,0,0,0,1,0,0}},
  {1, {0,0,0,0,0,0,1,0,0}}, // posisi home gerak ke kanan semua sensor nilai lebih dari batas
  {2, {0,0,0,0,0,0,1,0,0}},
  {3, {0,1,0,0,0,0,1,0,0}},
  {4, {0,0,0,0,0,0,1,0,0}},
  {5, {0,0,1,0,0,0,1,0,0}},
  {6, {0,0,0,1,0,0,1,0,1}},
  {7, {0,0,1,0,0,0,1,0,1}},
  {8, {0,0,0,0,0,0,1,0,1}},
  {9, {0,1,0,1,0,0,1,0,1}},
  {10, {0,0,0,0,0,0,1,0,1}},
  {11, {0,1,0,0,0,0,1,0,1}},
  {12, {0,0,0,1,0,0,1,0,1}},
  {13, {0,1,0,0,0,0,1,0,1}},
  {14, {1,0,1,0,0,0,1,0,1}},
  {15, {0,0,0,1,0,0,1,0,1}},
  {16, {0,1,0,0,0,0,1,0,1}},
  {17, {0,0,0,1,0,0,1,0,1}},
  {18, {0,0,0,0,0,0,1,0,1}},
  {19, {0,1,0,0,1,0,1,0,1}},
  {20, {0,1,0,0,0,0,1,0,1}},
  {21, {0,1,0,0,0,0,1,0,1}},
  {22, {0,0,0,0,0,0,1,0,1}},
  {23, {0,0,1,0,0,0,1,0,1}},
  {24, {0,0,0,1,1,0,1,0,1}},
  {25, {0,1,0,0,0,0,1,0,1}},
  {26, {0,0,0,1,0,0,1,0,1}},
  {27, {0,1,0,0,0,0,1,0,1}},
  {28, {0,0,1,0,1,0,1,0,1}},
  {29, {0,0,0,1,0,0,1,0,1}},
  {30, {0,0,0,0,0,0,1,0,1}},
  {31, {1,0,0,0,0,0,1,0,0}},
  {32, {0,1,0,0,0,0,1,0,1}},
  {33, {1,0,0,0,0,0,1,0,1}},
  {34, {1,0,1,0,1,0,1,0,1}},
  {35, {0,0,0,0,0,0,1,0,1}},
  {36, {0,1,0,0,0,0,1,0,1}},
  {37, {1,0,0,0,0,0,1,0,1}},
  {38, {0,0,0,0,0,0,1,0,1}},
  {39, {1,0,1,0,0,0,1,0,1}},
  {40, {0,0,0,0,0,0,1,0,1}},
  {41, {0,1,0,0,0,0,1,0,1}},
  {42, {0,0,0,1,0,0,1,0,1}},
  {43, {0,0,0,1,0,0,1,0,1}},
  {44, {0,1,0,0,0,0,1,0,1}},
  {45, {0,1,0,0,0,0,1,0,1}},
  {46, {0,0,0,0,0,0,1,0,1}},
  {47, {0,0,0,0,0,0,1,0,1}},
  {48, {1,0,0,0,0,0,1,0,1}},
  {49, {1,1,0,0,0,0,1,0,1}},
  {50, {0,0,0,0,0,0,1,0,1}},
  {51, {0,1,0,0,0,0,1,0,1}},
  {52, {0,1,0,1,0,0,1,0,1}},
  {53, {0,1,0,0,0,0,1,0,1}},
  {54, {1,1,1,1,0,0,1,0,1}},
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
  
  if (ping[0] <= 50 || ping[1] <=90 || ping[2] <= 40 || ping[3] <= 50) {
    isAvoidanceActive = true;
    if(ping[0] <= 50){
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
    twist.linear.x = 0.5;
    twist.linear.y = 0;
    twist.linear.z = 0;
    twist.angular.x = 0;
    twist.angular.y = 0;
    twist.angular.z = 0;

  }
  if(ping[3] <= 50){
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

    head_Tws.linear.x = xb * 0.7 ; //lifter
    head_Tws.linear.y = yb * turn ; //gripper

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
