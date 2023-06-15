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

int ping[8]={0,0,0,0,0,0,0,0};
// Depan kanan, Kanan , belakang kanan , depan , belakang , belakang kiri , kiri , depan kiri
void tofdistancesCallback(const std_msgs::Int32MultiArray::ConstPtr& msg)
{
  for (int i=0;i<8;i++){
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
char a_gerak[]  ={'D','w','s','x','D','d','w','s','a','w','a','w','s','d','w','d','s','a','w','A','w','d','w','A','a','D','w','s','x','A','w','d','w','s'}; 
//                 0,  1, 2,   3,  4,  5,  6,  7,  8,  9, 10,  11, 12, 13,14, 15, 16, 17, 18, 19, 20,  21,22, 23, 24, 25,  26,27, 28, 29, 30, 31,  32, 33
int gerak_1_[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

//program buat limit sensor dan gerakan kaki dan juga gerakan gripper
std::map<int, std::vector<int>> step{
  // {1, {0,0,-2,0,0,0,0,0,0.5,0.5}},   //batas 0-7, speed, turn  //rotate kanan
  //{step, {kanan_depan, kanan_tengah, kanan_belakang, depan, belakang, kiri_belakang, kiri_tengah, kiri_depan, Lifter, Gripper, speed, turn}}
  //{step, {kiri_depan, kiri_tengah, kiri_belakang, belakang, depan, kanan_belakang, kanan_tengah, kanan_depan, Lifter, Gripper, speed, turn}}
  {0, {0,538,0,0,278,0,0,0,-2,0,2,1}}, //keluar home
  {1, {0,0,0,220,99,0,0,0,0,-1,1,1}}, //ke k1
  {2, {0,0,0,0,99,0,0,0,0,0,1,1}}, //k1
  {3, {0,0,0,75,0,0,0,0,-2,0,1,1}}, //dari k1
  {4, {0,0,0,0,647,0,112,0,-2,0,2,1}}, //ke jalan retak, miring, batu
  {5, {80,0,0,0,220,0,0,157,-2,0,1,2}}, //ke sz1
  {6, {0,0,0,0,200,0,0,0,-2,0,1,1}},
  {7, {0,0,0,0,110,0,0,0,0,-1,1,1}}, //sz1
  {8, {0,0,0,350,664,0,120,0,-2,0,1,1}}, //arah kelereng
  {9, {0,215,0,560,416,0,0,0,-2,0,1,1}}, //ke k2
  {10, {0,0,0,175,190,0,395,0,-2,0,1,1}}, //hadap k2
  {11, {0,0,0,260,110,0,0,0,0,-1,1,1}}, //ke k2
  {12, {0,0,0,0,180,0,0,0,0,0,1,1}}, //k2
  {13, {0,0,0,0,378,310,210,170,-2,0,1,1}}, //arah sz2
  {14, {0,0,0,0,165,0,0,0,-2,0,1,1}}, //ke sz2
  {15, {0,0,0,485,180,0,0,0,0,-2,0,1,1}}, //ke sz2
  {16, {0,0,0,0,150,0,0,0,0,-1,1,1}}, //sz2 //s
  {17, {0,0,0,150,0,135,120,135,-2,0,1,1}}, //keluar sz2 //a
  {18, {0,340,0,750,0,0,0,0,-2,0,1,1}}, //menuju jalan retak
  {19, {0,100,0,100,0,0,0,0,-2,0,1,1}}, //menyamping sebelum ke jalan retak
  {20, {0,0,0,800,165,0,0,0,-2,0,1,1}}, //melewati jalan retak dan batu
  {21, {0,190,190,110,430,0,0,0,-2,0,1,1}}, //posisi siap naik tangga
  {22, {0,0,0,350,135,0,0,0,-2,0,1,1}}, //posisi siap naik tangga
  {23, {0,0,0,600,0,0,0,0,-2,0,1,1}}, //naik tangga
  {24, {0,0,0,0,0,0,0,0,0,-1,1,1}}, //
  {25, {0,0,0,0,0,0,0,0,0,0,1,1}}, //
  {26, {0,0,0,0,0,0,0,0,-2,0,1,1}}, //
  {27, {0,0,0,0,0,0,0,0,-2,0,1,1}}, //
  {28, {0,0,0,0,0,0,0,0,-2,0,1,1}}, //
  {29, {0,0,0,0,0,0,0,0,-2,0,1,1}}, //
  {30, {0,0,0,0,0,0,0,0,0,-1,1,1}}, //
  {31, {0,0,0,0,0,0,0,0,-2,0,1,1}}, //
  {32, {0,0,0,0,0,0,0,0,-2,0,1,1}}, //
  {33, {0,0,0,0,0,0,0,0,-2,0,1,1}}, //
};

std::map<int, std::vector<bool>> _f_{
  // ini program untuk kondisi if 1 atau 0 (komparasi)

  // {1, {0,0,1,0,0,0,0,0,0}},  //kompar 0-4 (0)(sensor>=batas) (1)(Sensor<=batas), LaserOrOdom(1=laser && 0=odom) //odom ,imu over , leg height
  //uneven = 0,1 && normal = 0,0 ( 2 digit terakhir) 
  {0, {0,0,0,0,0,0,0,0,1,0,0}}, // posisi home gerak ke kanan semua sensor nilai lebih dari batas
  {1, {0,0,0,0,1,0,0,0,1,0,0}},
  {2, {0,0,0,0,0,0,0,0,1,0,0}},
  {3, {0,0,0,1,0,0,0,0,1,0,0}},
  {4, {0,0,0,0,0,0,1,0,1,0,0}},
  {5, {1,0,0,0,0,0,0,0,1,0,0}},
  {6, {0,0,0,0,1,0,0,0,1,0,0}},
  {7, {0,0,0,0,1,0,0,0,1,0,0}},
  {8, {0,0,0,1,0,0,1,0,1,0,0}},
  {9, {0,1,0,0,1,0,0,0,1,0,0}},
  {10, {0,0,0,1,1,0,0,0,1,0,0}},
  {11, {0,0,0,0,1,0,0,0,1,0,0}},
  {12, {0,0,0,0,0,0,0,0,1,0,0}},
  {13, {0,0,0,0,0,1,1,1,1,0,0}},
  {14, {0,0,0,0,1,0,0,0,1,0,0}},
  {15, {0,0,0,1,0,0,0,0,1,0,0}},
  {16, {0,0,0,0,1,0,0,0,1,0,0}},
  {17, {0,0,0,1,0,1,1,1,1,0,0}},
  {18, {0,1,0,0,0,0,0,0,1,0,0}},
  {19, {0,1,0,1,0,0,0,0,1,0,0}},
  {20, {0,0,0,0,1,0,0,0,1,0,0}},
  {21, {0,0,0,1,0,0,0,0,1,0,0}},
  {22, {0,0,0,0,1,0,0,0,1,0,0}},
  {23, {0,0,0,0,0,0,0,0,1,0,1}},
  {24, {0,0,0,0,0,0,0,0,1,0,1}},
  {25, {0,0,0,0,0,0,0,0,1,0,1}},
  {26, {0,0,0,0,0,0,0,0,1,0,1}},
  {27, {0,0,0,0,0,0,0,0,1,0,1}},
  {28, {0,0,0,0,0,0,0,0,1,0,1}},
  {29, {0,0,0,0,0,0,0,0,1,0,1}},
  {30,  {0,0,0,0,0,0,0,0,1,0,1}},
  {31, {0,0,0,0,0,0,0,0,1,0,1}},
  {32, {0,0,0,0,0,0,0,0,1,0,1}},
  {33, {0,0,0,0,0,0,0,0,1,0,1}},

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
  //Settingan Pergerakan ngerubah
  if (ping[6] <= 50 || ping[4] <= 90 || ping[3] <= 40 || ping[1] <= 50) {
    isAvoidanceActive = true;
    if(ping[6] <= 50){
    //gerakan ke kiri
    twist.linear.x = 0;
    twist.linear.y = -0.5;
    twist.linear.z = 0;
    twist.angular.x = 0;
    twist.angular.y = 0;
    twist.angular.z = 0;
 
  }
  if(ping[4] <= 70 && xb == -2){
    //gerakan mundur
    twist.linear.x = -0.5;
    twist.linear.y = 0;
    twist.linear.z = 0;
    twist.angular.x = 0;
    twist.angular.y = 0;
    twist.angular.z = 0;

  }
  if(ping[4] <= 120 && xb == 0 && yb == 0){
    //gerakan mundur
    twist.linear.x = -0.5;
    twist.linear.y = 0;
    twist.linear.z = 0;
    twist.angular.x = 0;
    twist.angular.y = 0;
    twist.angular.z = 0;

  }
    if(ping[3] <= 40){
    //gerakan maju
    twist.linear.x = 0.5;
    twist.linear.y = 0;
    twist.linear.z = 0;
    twist.angular.x = 0;
    twist.angular.y = 0;
    twist.angular.z = 0;

  }
  if(ping[1] <= 50){
    //gerakan ke kanan
    twist.linear.x = 0;
    twist.linear.y = 0.5;
    twist.linear.z = 0;
    twist.angular.x = 0;
    twist.angular.y = 0;
    twist.angular.z = 0;

  }
  // if(ping[3]<= 50 && ping[1] <=70){
  //   //gerakan balik Kanan
  //   twist.linear.x = 0;
  //   twist.linear.y = 0;
  //   twist.linear.z = 0;
  //   twist.angular.x = 0;
  //   twist.angular.y = 0;
  //   twist.angular.z = -0.5;
  // }
  //   if(ping[0]<= 50 && ping[1] <=70){
  //   //gerakan balik Kiri
  //   twist.linear.x = 0;
  //   twist.linear.y = 0;
  //   twist.linear.z = 0;
  //   twist.angular.x = 0;
  //   twist.angular.y = 0;
  //   twist.angular.z = 0.5;
  // }
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
  int batas[8];
  if (step.count(step_) == 1)
    {
      for(int a=0;a<8;a++){
        batas[a]=step[step_][a];
      }
      // currentStep = step_;
      xb=step[step_][8];
      yb=step[step_][9];
      speed=step[step_][10];
      turn=step[step_][11];
    }

  bool flag_[8];
  if (_f_.count(step_) == 1)
    {
      for(int a=0;a<8;a++){
        flag_[a]=_f_[step_][a];
      }
    pilih=_f_[step_][8];
    imu_override_.data = _f_[step_][9];
    leg_height_.data = _f_[step_][10];
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
    
  
    ROS_INFO("%d, %d, %d, %d ,%d, %d, %d, %d", batas[0], batas[1], batas[2], batas[3], batas[4], batas[5], batas[6], batas[7]);
    ROS_INFO("%d, %d, %d, %d ,%d, %d, %d, %d",ping[0],ping[1],ping[2],ping[3],ping[4],ping[5],ping[6],ping[7]);
    ROS_INFO("%d, %d, %d, %d ,%d, %d, %d, %d",flag_[0],flag_[1],flag_[2],flag_[3],flag_[4],flag_[5],flag_[6],flag_[7]);


    bool s[4]={false,false,false,false};

  if(pilih==true){
    for (int a=0; a<8; a++){
    for (int a = 0; a < 8; a++) {
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

    for (int a=0; a<8; a++){
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
  
  if(s[0]==true && s[1]==true && s[2]==true && s[3]==true && s[4]==true && s[5]==true && s[6]==true && s[7]==true){
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
