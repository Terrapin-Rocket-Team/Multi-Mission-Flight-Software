#include "Barometer.h"
#include <cmath>
#include <Arduino.h>
namespace mmfs
{

#pragma region Barometer Specific Functions
    Barometer::Barometer(const char *name) : Sensor(name)
    {
        addColumn(DOUBLE, &pressure, "Pres (hPa)");
        addColumn(DOUBLE, &temp, "Temp (C)");
        addColumn(DOUBLE, &altitudeASL, "Alt ASL (m)");
        addColumn(DOUBLE, &altitudeAGL, "Alt AGL (m)");
    }

    Barometer::~Barometer() {}

    double Barometer::getPressure() const { return pressure; }

    double Barometer::getTemp() const { return temp; }

    double Barometer::getTempF() const { return (temp * 9.0 / 5.0) + 32.0; }

    double Barometer::getPressureAtm() const { return pressure / MEAN_SEA_LEVEL_PRESSURE_HPA; }

    double Barometer::getASLAltFt() const { return altitudeASL * 3.28084; }

    double Barometer::getASLAltM() const { return altitudeASL; }

    double Barometer::getAGLAltM() const { return altitudeAGL; }

    double Barometer::getAGLAltFt() const { return altitudeAGL * 3.28084; }

    double Barometer::calcAltitude(double pressure)
    {
        // Equation from NOAA, but for meters: https://www.weather.gov/media/epz/wxcalc/pressureAltitude.pdf
        return 44307.69 * (1.0 - pow(pressure / MEAN_SEA_LEVEL_PRESSURE_HPA, 0.190284));
    }

#pragma endregion // Barometer Specific Functions

#pragma region Sensor Virtual Function Implementations

    const char *Barometer::getTypeString() const { return "Barometer"; }

    const SensorType Barometer::getType() const { return BAROMETER_; }

    void Barometer::update()
    {
        read();
        altitudeASL = calcAltitude(pressure);
        if (biasCorrectionMode)
        {
            pressureBuffer.push(pressure);

            double sum = 0;
            int valsToCount = std::min(pressureBuffer.getCount(), CIRC_BUFFER_LENGTH - CIRC_BUFFER_IGNORE);
            for (int i = 0; i < valsToCount; i++)
            {
                sum += pressureBuffer[i];
            }
            groundPressure = sum / valsToCount / 1.0;
            groundAltitude = calcAltitude(groundPressure);
        }

        altitudeAGL = altitudeASL - groundAltitude;
    }

    bool Barometer::begin(bool useBiasCorrection)
    {
        pressure = 0;
        temp = 0;
        altitudeASL = 0;
        altitudeAGL = 0;
        groundPressure = 0;
        pressureBuffer.clear();
        biasCorrectionMode = useBiasCorrection;
        if (init())
        {
            if (!biasCorrectionMode)
            {
                double startPressure = 0;
                for (int i = 0; i < 25; i++)
                {
                    read();
#ifndef PIO_UNIT_TESTING // Don't delay in unit tests
                    delay(25);
#endif
                }

                for (int i = 0; i < 75; i++)
                {
                    read();
                    startPressure += pressure;
#ifndef PIO_UNIT_TESTING // Don't delay in unit tests
                    delay(25);
#endif
                }
                groundPressure = (startPressure / 75.0); // hPa
                groundAltitude = calcAltitude(groundPressure);
                printf("Ground Pressure: %.2f hPa\n", groundPressure);
                printf("Ground Altitude: %.2f m\n", groundAltitude);
                altitudeASL = groundAltitude;
            }
            for (int i = 0; i < 3; i++)
            {
                read();
                delay(50);
            }
            return true;
        }
        return false;
    }
}