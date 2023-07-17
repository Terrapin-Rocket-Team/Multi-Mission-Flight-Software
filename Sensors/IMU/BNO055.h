#ifndef BNO055_H
#define BNO055_H

#include <Adafruit_BNO055.h>

extern static Adafruit_BNO055 bno = Adafruit_BNO055(55);

bool setupBNO055();
void calibrate_bno();  // TODO only can use this if there is a timeout built in
imu::Vector<3> getBNO055acceleration();
imu::Vector<3> getBNO055angularVelocity();
imu::Vector<3> getBNO055magneticFieldStrength();
imu::Quaternion getBNO055absoluteOrientation();

imu::Quaternion quatMultiplication(imu::Quaternion quatL, imu::Quaternion quatR);

#endif