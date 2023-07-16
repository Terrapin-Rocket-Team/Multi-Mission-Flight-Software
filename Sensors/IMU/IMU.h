#ifndef IMU_H
#define IMU_H

#include <string>
#include "imumaths.h"  //Not needed if working with the Adafruit_BNO055 library
#include "BNO055.h"
#include "HighAccelBNO055.h"

class IMU {
    //Code written for ['BNO055', 'High Accel BNO055']
public:
    std::string sensorName;
    bool setupSuccessful;
    imu::Vector<3> acceleration;  //in m/s^2 relative to absolute reference frame
    imu::Vector<3> angularVelocity;  //in rad/s
    imu::Vector<3> magneticFieldStrength;  //in microTesla relative to the rocket reference frame
    imu::Quaternion absoluteOrientation;  //only used for data logging purposes, normailzed as well

    IMU();
    IMU(std::string inputSensorName);
    void setupIMU();
    void readIMU();

    void setcsvHeader();
    String getcsvHeader();
    void setdataString();
    String getdataString();

private:
    bool accelerationFlag;
    bool angularVelocityFlag;
    bool magneticFieldStrengthFlag;
    bool absoluteOrientationFlag;

    String csvHeader;
    String dataString;

};

#endif