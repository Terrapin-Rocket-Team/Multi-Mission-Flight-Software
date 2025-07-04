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
        virtual double getAGLAltM() const;
        virtual double getAGLAltFt() const;

        // Sensor virtual functions
        virtual void update() override;
        virtual bool begin(bool useBiasCorrection = true) override;

    protected:
        Barometer(const char *name = "Barometer");
        double pressure = 0;
        double temp = 0;

        // Altitude-related data
        double altitudeASL = 0;
        double altitudeAGL = 0;
        double groundPressure = 0;
        double groundAltitude = 0;

        CircBuffer<double> pressureBuffer = CircBuffer<double>(CIRC_BUFFER_LENGTH);

        double calcAltitude(double pressure);
    };
}
#endif // BAROMETER_H
