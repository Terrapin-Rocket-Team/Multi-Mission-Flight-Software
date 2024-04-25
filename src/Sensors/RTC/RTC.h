#ifndef RTC_H
#define RTC_H

#include <RTClib.h>
#include <imumaths.h>
#include "../Sensor.h"

// NOT READY FOR IMPLEMENTATION. NNEDS TO BE REWORKED TO MATCH OTHER SENSORS LIKE BARO, IMU, OR GPS
class RTC : public Sensor
{
public:
    virtual ~RTC(){};                                // virtual destructor
    virtual imu::Vector<2> getTimeOn() = 0;          // ms
    virtual imu::Vector<2> getTimeSinceLaunch() = 0; // ms
    virtual DateTime getLaunchTime() = 0;
    virtual DateTime setLaunchTime() = 0;
    virtual DateTime getPowerOnTime() = 0;
    virtual DateTime getCurrentTime() = 0;
    virtual SensorType getType() const override { return RTC_; }
    virtual const char *getTypeString() const override { return "RTC"; }
};

#endif
