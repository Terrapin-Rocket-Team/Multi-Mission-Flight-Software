#ifndef BAROMETER_H
#define BAROMETER_H

#include "../Sensor.h"

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
        virtual double getASLAltFt() const;
        virtual double getASLAltM() const;
        virtual double getAGLAltM() const;
        virtual double getAGLAltFt() const;
        virtual const char *getCsvHeader() const override;
        virtual const char *getDataString() const override;
        virtual const char *getStaticDataString() const override;
        virtual const char *getTypeString() const override { return "Barometer"; }
        virtual SensorType getType() const override { return BAROMETER_; }
        virtual void update() override;
        virtual void begin(bool useBiasCorrection = true) override;

    protected:
        Barometer()
        {                                                     // Protected constructor to prevent instantiation
            staticData = new char[25 + MAX_DIGITS_FLOAT * 1]; // 25 chars for the string, 12 chars for 1x float
            data = new char[MAX_DIGITS_FLOAT * 3 + 5];        // 12 chars for the 3x floats, 5 for the comma/null/buffer
        }

        double pressure = 0;       // hPa
        double temp = 0;           // C
        double altitudeASL = 0;    // m
        double altitudeAGL = 0;    // m
        double groundPressure = 0; // hPa
        double groundAltitude = 0; // m

        CircBuffer<double> pressureBuffer = CircBuffer<double>(UPDATE_RATE * SENSOR_BIAS_CORRECTION_DATA_LENGTH);

        double calcAltitude(double pressure);
    };
}
#endif // BAROMETER_H