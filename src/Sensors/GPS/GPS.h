#ifndef GPS_H
#define GPS_H

#include <imumaths.h>
#include "../Sensor.h"

class GPS : public Sensor
{
public:
    virtual ~GPS() {};
    virtual double getAlt() const;
    virtual imu::Vector<3> getVelocity() const;
    virtual imu::Vector<2> getPos() const;
    virtual imu::Vector<3> getOriginPos() const;
    virtual imu::Vector<3> getDisplacement() const;
    virtual const char *getTimeOfDay() const;
    virtual int getFixQual() const;
    virtual double getHeading() const;
    virtual bool getHasFirstFix() const;
    virtual const char *getCsvHeader() const override;
    virtual const char *getDataString() const override;
    virtual const char *getStaticDataString() const override;

    virtual const char *getTypeString() const override { return "GPS"; }
    virtual SensorType getType() const override { return GPS_; }

protected:
    GPS();
    imu::Vector<2> pos;          // latitude and longitude
    double altitude;             // alti in mm
    imu::Vector<3> velocity;     // m per s
    imu::Vector<3> displacement; // m from starting location
    imu::Vector<3> origin;       // lat(deg), long(deg), alti(m) of the original location
    int fixQual;                 // num of connections to satellites
    imu::Vector<3> irlTime;      // returns the current hour, min, and sec
    bool hasFirstFix;            // whether or not gps has recieved first fix
    double heading;
    int hr, min, sec;
    char gpsTime[9];
    double time;
};

#endif