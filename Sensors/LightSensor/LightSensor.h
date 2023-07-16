#ifndef LIGHTSENSOR_H
#define LIGHTSENSOR_H

#include <string>

#include "BH1750personal.h"

class LightSensor {
    //Code written for ['BH1750']
public:
    std::string sensorName;
    bool setupSuccessful;
    float lux;  //in lx

    LightSensor();
    LightSensor(std::string inputSensorName);
    void setupLightSensor();
    void readLightSensor();

    void setcsvHeader();
    String getcsvHeader();
    void setdataString();
    String getdataString();

private:
    bool luxFlag;

    String csvHeader;
    String dataString;

};

#endif