#include "GPS.h"


GPS::GPS()
{
    hasFirstFix = false;
    origin = imu::Vector<3>(0, 0, 0);
    pos = imu::Vector<2>(0, 0);
    velocity = imu::Vector<3>(0, 0, 0);
    displacement = imu::Vector<3>(0, 0, 0);
    irlTime = imu::Vector<3>(0, 0, 0);

    altitude = 0;
    fixQual = 0;
    heading = 0;
    hr = 0;
    min = 0;
    sec = 0;
    time = 0;
    strcpy(gpsTime, "00:00:00");
}

/*
return altitude in m
*/
double GPS::getAlt() const
{
    return altitude;
}

/*
returns the lat and long of the rocket to the 7th sig fig
*/
imu::Vector<2> GPS::getPos() const
{
    return pos;
}

double GPS::getHeading() const
{
    return heading;
}

/*
return the velocity (meters per second)
there probably issues with floating points
*/
imu::Vector<3> GPS::getVelocity() const
{
    return velocity;
}

/*
retern the displacement since the origin
there is probably issues with floating point arithmetic
*/
imu::Vector<3> GPS::getDisplacement() const
{
    return displacement;
}

/*
returns vector of orginal position in lat(deg), lat(deg), and alti(m)
*/
imu::Vector<3> GPS::getOriginPos() const
{
    return origin;
}

bool GPS::getHasFirstFix() const
{
    return hasFirstFix;
}

/*
time since in initialization in seconds
*/
const char *GPS::getTimeOfDay() const
{
    return gpsTime;
}

/*
return the number of satellites to indicate quality of data
*/
int GPS::getFixQual() const
{
    return fixQual;
}

const char *GPS::getCsvHeader() const
{                                                                                                                        // incl G- for GPS
    return "G-Lat (deg),G-Lon (deg),G-Alt (m),G-Speed (m/s),G-DispX (m),G-DispY (m),G-DispZ (m),G-ToD (s),G-# of Sats,"; // trailing comma
}

const char *GPS::getDataString() const
{
    // See State.cpp::setDataString() for comments on what these numbers mean. 15 for GPS.
    const int size = 15 * 2 + 12 * 4 + 10 * 1 + 10 + 8;
    char *data = new char[size];
    snprintf(data, size, "%.10f,%.10f,%.2f,%.2f,%.2f,%.2f,%.2f,%s,%d,", pos.x(), pos.y(), altitude, velocity.magnitude(), displacement.x(), displacement.y(), displacement.z(), gpsTime, fixQual); // trailing comma
    return data;
}

const char *GPS::getStaticDataString() const
{
    // See State.cpp::setDataString() for comments on what these numbers mean. 15 for GPS.
    const int size = 60 + 15 * 2 + 12 * 1;
    char *data = new char[size];
    snprintf(data, size, "Original Latitude (m): %.10f\nOriginal Longitude (m): %.10f\nOriginal Altitude (m): %.2f\n", origin.x(), origin.y(), origin.z());
    return data;
}