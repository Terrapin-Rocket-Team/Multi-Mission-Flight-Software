#ifndef BAROMETER_H
#define BAROMETER_H

#include "../Sensor.h"

#define SEA_LEVEL_PRESSURE_HPA (1013.25)

namespace mmfs
{
    class Barometer : public Sensor
    {
    public:
        virtual ~Barometer() {}
        virtual double getPressure() const;
        virtual double getTemp() const;
        virtual double getTempF() const;
        virtual double getPressureAtm() const;
        virtual double getRelAltFt() const;
        virtual double getRelAltM() const;
        virtual const char *getCsvHeader() const override;
        virtual const char *getDataString() const override;
        virtual const char *getStaticDataString() const override;
        virtual const char *getTypeString() const override { return "Barometer"; }
        virtual SensorType getType() const override { return BAROMETER_; }

    protected:
        Barometer()
        {                                       // Protected constructor to prevent instantiation
            staticData = new char[25 + MAX_DIGITS_FLOAT * 1]; // 25 chars for the string, 12 chars for the float
            data = new char[MAX_DIGITS_FLOAT * 3 + 3];        // 12 chars for the 3 floats, 3 for the comma
        }
        double pressure = 0;
        double temp = 0;
        double altitude = 0;
        double groundPressure = 0;
    };
}
#endif // BAROMETER_H