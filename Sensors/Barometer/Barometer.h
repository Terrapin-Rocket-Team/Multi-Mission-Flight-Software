#ifndef BAROMETER_H
#define BAROMETER_H

#include <string>

#include "BMP280.h"
#include "BMP280.cpp"

class Barometer {
    //Code written for ['BMP280']
public:
    std::string sensorName;
    bool setupSuccessful;
    double relativeAltitude;  //in meters above start
    double pressure;  //in Pa
    double temperature;  //in degrees C

    Barometer();
    Barometer(std::string inputSensorName);
    void setupBarometer();
    void readBarometer();

    void setcsvHeader();
    String getcsvHeader();
    void setdataString();
    String getdataString();

private:
    bool relativeAltitudeFlag;
    bool pressureFlag;
    bool temperatureFlag;

    String csvHeader;
    String dataString;

};

#endif