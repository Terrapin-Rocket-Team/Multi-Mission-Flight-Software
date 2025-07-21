#include "Barometer.h"
#include <cmath>
#include <Arduino.h>
namespace mmfs
{

#pragma region Barometer Specific Functions
    Barometer::Barometer(const char *name) : Sensor("Barometer", name)
    {
        addColumn(DOUBLE, &pressure, "Pres (hPa)");
        addColumn(DOUBLE, &temp, "Temp (C)");
        addColumn(DOUBLE, &altitudeASL, "Alt ASL (m)");
    }

    Barometer::~Barometer() {}

    double Barometer::getPressure() const { return pressure; }

    double Barometer::getTemp() const { return temp; }

    double Barometer::getTempF() const { return (temp * 9.0 / 5.0) + 32.0; }

    double Barometer::getPressureAtm() const { return pressure / MEAN_SEA_LEVEL_PRESSURE_HPA; }

    double Barometer::getASLAltFt() const { return altitudeASL * 3.28084; }

    double Barometer::getASLAltM() const { return altitudeASL; }

    double Barometer::calcAltitude(double pressure)
    {
        // Equation from NOAA, but for meters: https://www.weather.gov/media/epz/wxcalc/pressureAltitude.pdf
        return 44307.69 * (1.0 - pow(pressure / MEAN_SEA_LEVEL_PRESSURE_HPA, 0.190284));
    }

#pragma endregion // Barometer Specific Functions

#pragma region Sensor Virtual Function Implementations

    bool Barometer::update()
    {
        if (!read())
            return false;
        altitudeASL = calcAltitude(pressure);
        return true;
    }

    bool Barometer::begin()
    {
        pressure = 0;
        temp = 0;
        altitudeASL = 0;
        if(initialized = init())
        {
            read();
            altitudeASL = calcAltitude(pressure);
        }
        return initialized;
    }
}