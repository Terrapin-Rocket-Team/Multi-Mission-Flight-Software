#ifndef GPS_H
#define GPS_H

#include "../Sensor.h"
#include "../../Constants.h"
#include "../../Math/Vector.h"
#include "../../RecordData/Logging/Logger.h"
#include "../../Events/DefaultEvents.h"

namespace mmfs
{
    class GPS : public Sensor
    {
    public:
        virtual ~GPS();
        virtual Vector<3> getPos() const;
        virtual int getFixQual() const;
        virtual double getHeading() const;
        virtual bool getHasFix() const;
        virtual bool update() override;
        virtual bool begin() override;
        virtual Vector<3> getDisplacement(Vector<3> origin) const;

        virtual const char *getTimeOfDay() const;

        virtual int8_t getHour() const;
        virtual int8_t getMinute() const;
        virtual int8_t getSecond() const;
        virtual uint8_t getDay() const;
        virtual uint8_t getMonth() const;
        virtual uint16_t getYear() const;

    protected:
        GPS(const char *name = "GPS");
        Vector<3> position;     // latitude, longitude, alt(m)
        int fixQual = 0;        // number of satellite connections
        bool hasFix;            // whether or not GPS is currently connected to >= 4 satellites
        bool hasFirstFix;       // the first time it gets a fix
        double heading = 0;

        // Distance-related calculations
        void calcInitialValuesForDistance();
        double kx, ky;
        double wrapLongitude(double val) const;
        void findTimeZone();

        char tod[9];

        int8_t hr = 0, min = 0, sec = 0;
        uint8_t day = 0, month = 0;
        uint16_t year = 0;
        int8_t hrOffset = 0;
    };
}

#endif // GPS_H
