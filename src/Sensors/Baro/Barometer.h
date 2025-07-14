#ifndef BAROMETER_H
#define BAROMETER_H

#include "../Sensor.h"

namespace mmfs
{
    class Barometer : public Sensor
    {
    public:
        virtual ~Barometer();
        virtual double getPressure() const;
        virtual double getTemp() const;
        virtual double getTempF() const;
        virtual double getPressureAtm() const;
        virtual double getASLAltFt() const;
        virtual double getASLAltM() const;

        // Sensor virtual functions
        virtual bool update() override;
        virtual bool begin() override;

    protected:
        Barometer(const char *name = "Barometer");
        double pressure = 0;
        double temp = 0;

        // Altitude-related data
        double altitudeASL = 0;

        double calcAltitude(double pressure);
    };
}
#endif // BAROMETER_H
