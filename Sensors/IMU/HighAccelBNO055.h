#ifndef HIGH_ACCEL_BNO055_H
#define HIGH_ACCEL_BNO055_H

#include <Adafruit_BNO055.h>

static Adafruit_BNO055 highAccelbno = Adafruit_BNO055(55);

bool setupHighAccelBNO055();
void calibrateHighAccelBNO();  // TODO only can use this if there is a timeout built in
imu::Vector<3> getHighAccelBNO055acceleration();
imu::Vector<3> getHighAccelBNO055angularVelocity();
imu::Vector<3> getHighAccelBNO055magneticFieldStrength();

#endif
