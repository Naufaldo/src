#include "ros/ros.h"
#include "std_msgs/String.h"

#include <nav_msgs/Odometry.h>
#include <std_msgs/Int32MultiArray.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/AccelStamped.h>
#include <geometry_msgs/PoseStamped.h>
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

float ping[7]={0,0,0,0,0,0,0};
// depan, belakang, kiri, kanan, roll, pitch, yaw
void tofdistancesCallback(const std_msgs::Int32MultiArray::ConstPtr& msg)
{
  for (int i=0;i<4;i++){
    ping[i]=msg->data[i];
  }
   ROS_INFO("ToF Reading: front=%f back=%f  left=%f right=%f", ping[0], ping[1], ping[2], ping[3]);
}

void eulerCallback(const geometry_msgs::PoseStamped::ConstPtr& msg)
{
  // Extract Euler angles from the received message
  ping[4] = std::abs(msg->pose.position.x);     // roll orientation
  ping[5] = std::abs(msg->pose.position.y);     // pitch orientation
  ping[6] = std::abs(msg->pose.position.z);     // yaw orientation

  ROS_INFO("IMU Orientation: roll=%f pitch=%f yaw=%f", ping[4], ping[5], ping[6]);
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
char a_gerak[]  ={'d','d','w','a','w','s','s','x','d','w','w','w','A','w','d','w','s','x','x','d','w','s','s','x','d','w','D','w','s','x','a','w','D','w','A','w','w','d','w','A','a'};
// step            0   1   2   3   4   5   6   7   8   9  10   11  12  13  14  15  16  17  18  19  20  21  22  23  24  25  26  27  28  29  30  31  32  33  34  35  36  37  38  39

// Pengondisian step dan batas gerakan
std::map<int, std::vector<float>> step{
  // {step, {Tof_depan, Tof_belakang, Tof_kiri, Tof_kanan, Imu Roll, Imu Pitch, Imu Yaw, Gripper (lifter), Gripper (gripper), Speed, Turn}}
  // gripper: teleop 'o' --> {0,0}; teleop 'p' --> {-2,0}; teleop 'l' --> {0,-1}; teleop ';' --> {-1,-1}
  {0,   {0,0,0,0,0,0,20,            -2,0,1,1}},   // keluar dari home (handling pengondisian untuk orientasi berbeda)
  {1,   {500,0,100,100,0,0,90,      -2,0,1,1}},   // keluar dari home
  {2,   {0,520,220,0,0,0,0,         -2,0,2,1}},   // menuju zona K1
  {3,   {250,125,470,500,0,0,5,   -2,0,1,1}},   // berotasi hingga gripper sejajar K1
  {4,   {150,190,0,0,0,0,0,         0,-1,1,1}},   // mendekati K1 (gripper diturunkan dan terbuka) 
  {5,   {140,0,0,0,0,0,0,            0,0,1,1}},   // di posisi K1 dan gripper men-grip korban
  {6,   {140,0,0,0,0,0,0,           -2,0,1,1}},   // gripper dengan korban diangkat kembali
  {7,   {0,100,0,0,0,0,0,           -2,0,1,1}},   // keluar dari zona K1 
  {8,   {0,500,300,150,0,0,90,      -2,0,1,1}},   // berotasi sejajar jalur utama
  {9,   {0,580,0,0,0,0,0,           -2,0,2,1}},   // bergerak maju hingga ke R1 (Jalan Retak)
  {10,  {0,0,0,0,0,10,0,            -2,0,2,1}},   // bergerak maju di R1 (Jalan Retak)
  {11,  {300,0,0,0,0,0,0,           -2,0,2,1}},   // bergerak maju melewati R2 (Turunan) dan R3 (Bebatuan) - Menuju SZ1
  {12,  {0,0,70,0,0,0,0,            -2,0,1,1}},   // menyamping ke kiri sebelum ke SZ1
  {13,  {100,0,400,0,0,0,0,         -2,0,1,1}},   // maju dan bersiap ke SZ1
  {14,  {0,0,120,0,0,0,175,         -2,0,1,1}},   // berotasi hingga sejajar dengan SZ1
  {15,  {220,500,0,0,0,0,0,         -2,0,1,1}},   // mendekat pada SZ1
  {16,  {100,0,0,0,0,0,0,           0,-1,1,1}},   // menurunkan K1 di SZ1
  {17,  {0,500,0,0,0,0,0,           0,-1,1,1}},   // mundur dengan gripper masih terbuka (handling korban terangkat lagi)
  {18,  {0,430,0,0,0,0,0,           -2,0,1,1}},   // mundur hingga tegak lurus K2
  {19,  {290,0,0,280,0,0,90,       -2,0,1,1}},   // berotasi hingga gripper sejajar K2
  {20,  {110,180,0,0,0,0,         0,-1,1,1}},   // mendekati K2 (gripper diturunkan dan terbuka) 
  {21,  {110,0,0,0,0,0,            0,0,1,1}},   // di posisi K2 dan gripper men-grip korban
  {22,  {120,0,0,0,0,0,           -2,0,1,1}},   // gripper dengan korban diangkat kembali
  {23,  {0,70,0,0,0,0,            -2,0,1,1}},   // bergerak mundur
  {24,  {0,400,0,50,0,0,          -2,0,1,1}},   // berotasi hingga sejajar dengan SZ2
  {25,  {0,200,0,0,0,0,0,           -2,0,1,1}},   // maju menuju SZ2
  {26,  {0,0,0,50,0,0,5,            -2,0,1,1}},   // menyamping mendekati tembok agar sejajar dengan SZ2
  {27,  {0,170,0,0,0,0,0,           -2,0,1,1}},   // maju menuju SZ2
  {28,  {100,0,0,0,0,0,0,           0,-1,1,1}},   // menurunkan K2 di SZ2
  {29,  {0,500,0,0,0,0,0,           0,-1,1,1}},   // mundur dengan gripper masih terbuka (handling korban terangkat lagi)
  {30,  {0,0,0,0,0,0,90,            -2,0,1,1}},   // berotasi hingga tegak lurus SZ2
  {31,  {0,250,370,0,0,0,0,         -2,0,1,1}},   // maju keluar SZ2
  {32,  {0,0,0,80,0,0,0,            -2,0,1,1}},   // menyamping keluar SZ2
  {33,  {0,500,300,0,0,0,0,         -2,0,1,1}},   // maju keluar SZ2
  {34,  {0,0,100,0,0,0,0,           -2,0,1,1}},   // menyamping memposisikan diri menuju R? (Jalan Retak)
  {35,  {0,350,0,0,0,0,0,           -2,0,1,1}},   // maju menuju R? (Jalan Retak)
  {36,  {100,500,0,0,0,0,0,         -2,0,1,1}},   // maju melewati R? (Jalan Retak) dan R? (Bebatuan)
  {37,  {0,0,0,0,0,0,5,             -2,0,1,1}},   // berotasi memposisikan diri sejajar dengan tangga
  {38,  {100,300,0,0,0,0,0,         -2,0,1,1}},   // maju hingga body robot collinear dengan tangga
  {39,  {0,400,200,0,10,0,0,        -2,0,1,1}},   // melewati tangga
  {40,  {0,0,240,300,0,0,90,       -2,0,1,1}},
};

std::map<int, std::vector<bool>> _f_{
  // komparator (0)(sensor>=batas) (1)(Sensor<=batas) (index 0 - 7)
  // uneven = (0, 1) && normal = (0, 0) (index 9 - 10) 
  {0,   {0,0,0,0,0,0,0, 1,0,0}},
  {1,   {0,0,1,1,0,0,0, 1,0,0}},
  {2,   {0,0,0,0,0,0,0, 1,0,0}},
  {3,   {1,1,0,0,0,0,1, 1,0,0}}, 
  {4,   {1,0,0,0,0,0,0, 1,0,0}},
  {5,   {1,0,0,0,0,0,0, 1,0,0}},
  {6,   {0,0,0,0,0,0,0, 1,0,0}},
  {7,   {0,1,0,0,0,0,0, 1,0,0}},
  {8,   {0,0,1,1,0,0,0, 1,0,0}},
  {9,   {0,0,0,0,0,0,0, 1,0,0}},
  {10,  {0,0,0,0,0,0,0, 1,0,1}},
  {11,  {1,0,0,0,0,0,0, 1,0,0}},
  {12,  {0,0,1,0,0,0,0, 1,0,0}},
  {13,  {1,0,0,0,0,0,0, 1,0,0}},
  {14,  {0,0,1,0,0,0,0, 1,0,0}},
  {15,  {1,0,0,0,0,0,0, 1,0,0}},
  {16,  {1,0,0,0,0,0,0, 1,0,0}},
  {17,  {0,1,0,0,0,0,0, 1,0,0}},
  {18,  {0,1,0,0,0,0,0, 1,0,0}},
  {19,  {1,0,0,0,0,0,0, 1,0,0}},
  {20,  {1,0,0,0,0,0,0, 1,0,0}},
  {21,  {1,0,0,0,0,0,0, 1,0,0}},
  {22,  {0,0,0,0,0,0,0, 1,0,0}},
  {23,  {0,1,0,0,0,0,0, 1,0,0}},
  {24,  {0,0,0,1,0,0,1, 1,0,0}},
  {25,  {0,1,0,0,0,0,1, 1,0,0}},
  {26,  {0,0,0,1,0,0,1, 1,0,0}},
  {27,  {0,1,0,0,0,0,1, 1,0,0}},
  {28,  {1,0,0,0,0,0,0, 1,0,0}},
  {29,  {0,1,0,0,0,0,0, 1,0,0}},
  {30,  {0,0,0,0,0,0,0, 1,0,0}},
  {31,  {0,0,1,0,0,0,0, 1,0,0}},
  {32,  {0,0,0,1,0,0,0, 1,0,0}},
  {33,  {0,0,0,0,0,0,0, 1,0,0}},
  {34,  {0,0,1,0,0,0,0, 1,0,0}},
  {35,  {0,0,0,0,0,0,0, 1,0,0}},
  {36,  {1,0,0,0,0,0,0, 1,0,1}},
  {37,  {0,0,0,0,0,0,1, 1,0,1}},
  {38,  {1,0,0,0,0,0,0, 1,0,1}},
  {39,  {0,0,1,0,1,0,0, 1,0,1}},
  {40,  {0,0,0,1,0,0,0, 1,0,1}},
};

// Init variables
float speed(1);                                                     // Linear velocity (m/s)
float turn(1);                                                      // Angular velocity (rad/s)
float x(0), y(0), z(0), xa(0), ya(0), za(0), xb(0), yb(0), th(0);   // Forward/backward/neutral direction vars
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
  }else {
    isAvoidanceActive = false;
  }
}

bool pilih;
void kontrol(char arah_, int step_){
  if (isAvoidanceActive) {
    return;
  }
  if (step_ == 0)
  {
    // Conditioning for robot first orientation
    if(ping[3] > ping[2])         // if the right's measurement greater than left's
    {
      key = arah_;
      diffOrient = false;
    }
    else if(ping[3] > ping[2])    // if the right's measurement smaller than left's
    {
      key = 'a';
      diffOrient = true;
    }
  }
  else if (step_ == 1 && diffOrient)
  {
    key = 'a';
  }
  else
  {
    key = arah_;
  }
  int batas[7];
  if (step.count(step_) == 1)
    {
      for(int a=0;a<7;a++){
        batas[a]=step[step_][a];
        if (diffOrient)
        {
          if (step_ == 3)
          {
            batas[6] = 175;
          }
          else if (step_ == 14)
          {
            batas[6] = 5;
          }
          else if (step_ == 24)
          {
            batas[6] = 175;
          }
          else if (step_ == 37)
          {
            batas[6] = 175;
          }
        }
      }
      // currentStep = step_;
      xb=step[step_][7];
      yb=step[step_][8];
      speed=step[step_][9];
      turn=step[step_][10];
    }

  bool flag_[7];
  if (_f_.count(step_) == 1)
    {
      for(int a=0;a<7;a++){
        flag_[a]=_f_[step_][a];
        if (diffOrient)
        {
          if (step_ == 3)
          {
            flag_[6] = 0;
          }
          else if (step_ == 8)
          {
            flag_[6] = 1;
          }
          else if (step_ == 14)
          {
            flag_[6] = 1;
          }
          else if (step_ == 19)
          {
            flag_[6] = 0;
          }
          else if (step_ == 24)
          {
            flag_[6] = 0;
          }
          else if (step_ == 30)
          {
            flag_[6] = 1;
          }
          else if (step_ == 37)
          {
            flag_[6] = 0;
          }
          else if (step_ == 40)
          {
            flag_[6] = 0;
          }
        }
      }
    pilih=_f_[step_][7];
    imu_override_.data = _f_[step_][8];
    leg_height_.data = _f_[step_][9];
    }
    

  // Trying to stabilize the orientation of robot's body
  if (moveBindings.count(key) == 1 && (step_ == 10 || step_ == 36))
  {
    float desired_yaw = 90.0;
    float yaw_difference = desired_yaw - ping[6];
    if (std::abs(yaw_difference) > 5.0) 
    {
      // If the yaw difference is greater than 5 degrees, rotate until desired orientation is reached
      twist.angular.z = (yaw_difference > 0) ? 0.5 : -0.5; // Adjust the angular velocity as needed
      // Keep linear velocities zero
      twist.linear.x = 0;
      twist.linear.y = 0;
      twist.linear.z = 0;
    }
    else
    {
      // Grab the direction data
      x = moveBindings[key][0];
      y = moveBindings[key][1];
      z = moveBindings[key][2];
      th = moveBindings[key][3];
      imu_override_.data = false;
           
      ROS_INFO("\rCurrent: speed %f   | turn %f | Last command: %c   ", speed, turn, key);
    }
  }
  else
  {
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
    
  
    ROS_INFO("%d, %d, %d, %d, %d, %d, %d ", batas[0], batas[1], batas[2], batas[3], batas[4], batas[5], batas[6]);
    ROS_INFO("%f, %f, %f, %f, %f, %f, %f",ping[0],ping[1],ping[2],ping[3],ping[4],ping[5],ping[6]);
    ROS_INFO("%d, %d, %d, %d, %d, %d, %d",flag_[0],flag_[1],flag_[2],flag_[3],flag_[4],flag_[5],flag_[6]);


    bool s[6]={false,false,false,false,false,false};

  if(pilih==true){
    for (int a=0; a<7; a++){
    for (int a = 0; a < 7; a++) {
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

    for (int a=0; a<7; a++){
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
  
  if(s[0]==true && s[1]==true && s[2]==true && s[3]==true && s[4]==true && s[5]==true && s[6]==true){
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
  ros::Subscriber euler_sub = n.subscribe("/euler_topic", 10, eulerCallback);
  ros::Subscriber sub = n.subscribe("/tof_distances", 10, tofdistancesCallback);

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
      // pub_pompa.publish(asd);

      // ROS_INFO("step: %s", qwerty.data);
      ROS_INFO("step: %d, %d", flag1);


    ros::spinOnce();
    r.sleep();
  }
  return 0;
}
