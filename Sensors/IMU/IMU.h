// Placeholder file for the IMU class

#ifndef IMU_H
#define IMU_H

#include "../Sensor.h"
#include <imumaths.h>

class IMU : public Sensor
{
public:
    virtual ~IMU(){};
    virtual imu::Quaternion getOrientation();
    virtual imu::Vector<3> getAcceleration();
    virtual imu::Vector<3> getOrientationEuler();
    virtual imu::Vector<3> getMagnetometer();
    virtual SensorType getType() const override { return IMU_; }
    virtual const char *getTypeString() const override { return "IMU"; }
    virtual const char *getCsvHeader() const override;
    virtual const char *getDataString() const override;
    virtual const char *getStaticDataString() const override;

protected:
    IMU(){}; // Protected constructor to prevent instantiation
    imu::Vector<3> accelerationVec = imu::Vector<3>(0, 0, 0);
    imu::Vector<3> orientationEuler = imu::Vector<3>(0, 0, 0);
    imu::Quaternion orientation = imu::Quaternion(1, 0, 0, 0);
    imu::Vector<3> magnetometer = imu::Vector<3>(0, 0, 0);
    imu::Vector<3> initialMagField = imu::Vector<3>(0, 0, 0);
};

imu::Vector<3> convertToEuler(const imu::Quaternion &orientation);

#endif