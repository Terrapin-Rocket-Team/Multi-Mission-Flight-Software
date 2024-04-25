#include "Barometer.h"


double Barometer::getPressure() const
{
    return pressure;
}

double Barometer::getTemp() const
{
    return temp;
}

double Barometer::getTempF() const
{
    return (temp * 9.0 / 5.0) + 32.0;
}

double Barometer::getPressureAtm() const
{
    return pressure / SEA_LEVEL_PRESSURE_HPA;
}

double Barometer::getRelAltM() const
{
    return altitude;
}

double Barometer::getRelAltFt() const
{
    return altitude * 3.28084;
}

const char *Barometer::getCsvHeader() const
{                                                 // incl  B- to indicate Barometer data  vvvv Why is this in ft and not m?
    return "B-Pres (hPa),B-Temp (C),B-Alt (ft),"; // trailing commas are very important
}

const char *Barometer::getDataString() const
{ // See State.cpp::setDataString() for comments on what these numbers mean
    // float x3
    const int size = 12 * 3 + 3;
    char *data = new char[size];
    snprintf(data, size, "%.2f,%.2f,%.2f,", pressure, temp, getRelAltFt()); // trailing comma
    return data;
}

const char *Barometer::getStaticDataString() const
{ // See State.cpp::setDataString() for comments on what these numbers mean
    const int size = 25 + 12 * 1;
    char *data = new char[size];
    snprintf(data, size, "Ground Pressure (hPa): %.2f\n", groundPressure);
    return data;
}