#include "IMU.h"

using namespace mmfs;

imu::Quaternion IMU::getOrientation()
{
    return orientation;
}

imu::Vector<3> IMU::getAcceleration()
{
    return accelerationVec;
}

imu::Vector<3> IMU::getOrientationEuler()
{
    return orientationEuler;
}

imu::Vector<3> IMU::getMagnetometer()
{
    return magnetometer;
}

imu::Vector<3> convertToEuler(const imu::Quaternion &orientation)

{
    imu::Vector<3> euler = orientation.toEuler();
    // reverse the vector, since it returns in z, y, x
    euler = imu::Vector<3>(euler.x(), euler.y(), euler.z());
    return euler;
}

const char *IMU::getCsvHeader() const
{                                                                                                          // incl I- for IMU
    return "I-AX (m/s/s),I-AY (m/s/s),I-AZ (m/s/s),I-ULRX,I-ULRY,I-ULRZ,I-QUATX,I-QUATY,I-QUATZ,I-QUATW,"; // trailing comma
}

const char *IMU::getDataString() const
{
    sprintf(data, "%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,", accelerationVec.x(), accelerationVec.y(), accelerationVec.z(), orientationEuler.x(), orientationEuler.y(), orientationEuler.z(), orientation.x(), orientation.y(), orientation.z(), orientation.w()); // trailing comma"
    return data;
}

const char *IMU::getStaticDataString() const
{
    sprintf(data, "Initial Magnetic Field (uT): %.2f,%.2f,%.2f\n", initialMagField.x(), initialMagField.y(), initialMagField.z());
    return data;
}