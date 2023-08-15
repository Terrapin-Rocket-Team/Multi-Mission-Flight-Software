#include "nav.h"

void servoSetup(int s1, int s2,int s3,int s4,int u1,int u2,int u3,int u4){ //input the servo pins, and the value for the servo to be up
  neSer.attach(s1);
  nwSer.attach(s2);
  swSer.attach(s3);
  seSer.attach(s4);
  neSer.write(u1);
  nwSer.write(u2);
  swSer.write(u3);
  seSer.write(u4);
}

void moveServo(double p1, double p2, double p3, double p4){
  neSer.write(p1);
  nwSer.write(p2);
  swSer.write(p3);
  seSer.write(p4);
}

imu::Vector<3> quatToXYZ(){
  imu::Vector<3> XYZ;
  imu::Quaternion q = bno.getBNO055absoluteOrientation(); //function from BNO55.cpp
  double qw = q.w(), qx = q.x(), qy = q.y(), qz = q.z();
  XYZ.x() = atan2(2 * (qw*qx + qy*qz), 1 - 2 * (qx*qx + qy*qy));
  XYZ.y() = asin(2 * (qw*qy - qz*qx));
  XYZ.z() = atan2(2 * (qw*qz + qx*qy), 1 - 2 * (qy*qy + qz*qz));
  XYZ.toDegrees(); //function from vector.h
  return XYZ;
}

void goDirection(string D, double neg){
  imu::Quaternion ori = bno.getBNO055absoluteOrientation(); //function from BNO55.cpp
  double goal = 0; //default set to North
  double yaw = ori.x();
  double pitch = ori.y();
  double roll = ori.z();
  if(D == "North") goal = 0;
  else if(D == "West") goal = 90;
  else if(D == "South") goal = 180;
  else if(D == "East") goal = -90;
  goal = goal - yaw; //how much you have to turn

  //makes sure that goal is within (-180, 180]
  while(goal <= -180) goal += 360;
  while(goal > 180) goal = goal - 360;

  if(goal>=-45 && goal <=45){ //Move Forward
    if(fabs(roll) < neg){//negligible roll
      if(pitch>(-neg)){
        moveServo(0,0,0,0);
      }
      else{
        moveServo(0,0,0,0);
      }
    }
    else if(roll>= neg){
      moveServo(0,0,0,0);
    }
    else{
      moveServo(0,0,0,0);
    }
  }
  else if(goal=>45 && goal <=135){ //Move Left
    if(fabs(roll) < neg){//negligible roll
      if(pitch>(-neg)){
        moveServo(0,0,0,0);
      }
      else{
        moveServo(0,0,0,0);
      }
    }
    else if(roll>= neg){
      moveServo(0,0,0,0);
    }
    else{
      moveServo(0,0,0,0);
    }
  }
  else if(goal>=-135 && goal <=-45){ //Move Right
    if(fabs(roll) < neg){//negligible roll
      if(pitch>(-neg)){
        moveServo(0,0,0,0);
      }
      else{
        moveServo(0,0,0,0);
      }
    }
    else if(roll>= neg){
      moveServo(0,0,0,0);
    }
    else{
      moveServo(0,0,0,0);
    }
  }
  else{ //Move Backwards
    if(fabs(roll) < neg){//negligible roll
      if(pitch>(-neg)){
        moveServo(0,0,0,0);
      }
      else{
        moveServo(0,0,0,0);
      }
    }
    else if(roll>= neg){
      moveServo(0,0,0,0);
    }
    else{
      moveServo(0,0,0,0);
    }
  }
}
