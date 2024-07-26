#ifndef GPS_H
#define GPS_H

#include "../Sensor.h"
#include "../../Constants.h"
#include "../../Math/Vector.h"

namespace mmfs
{
    class GPS : public Sensor
    {
    public:
        virtual ~GPS(){};
        virtual double getAlt() const;
        virtual Vector<3> getVelocity() const;
        virtual Vector<2> getPos() const;
        virtual Vector<3> getOriginPos() const;
        virtual Vector<3> getDisplacement() const;
        virtual const char *getTimeOfDay() const;
        virtual int getFixQual() const;
        virtual double getHeading() const;
        virtual bool getHasFirstFix() const;
        virtual const char *getCsvHeader() const override;
        virtual const char *getDataString() const override;
        virtual const char *getStaticDataString() const override;
        virtual void update() override;

        virtual const char *getTypeString() const override { return "GPS"; }
        virtual SensorType getType() const override { return GPS_; }

    protected:
        GPS();
        Vector<2> pos;          // latitude and longitude
        double altitude;             // alti in mm
        Vector<3> velocity;     // m per s
        Vector<3> displacement; // m from starting location
        Vector<3> origin;       // lat(deg), long(deg), alti(m) of the original location
        int fixQual;                 // num of connections to satellites
        Vector<3> irlTime;      // returns the current hour, min, and sec
        bool hasFirstFix;            // whether or not gps has recieved first fix
        double heading;
        int hr, min, sec;
        char gpsTime[9];
        double time;
    };
}

#endif