#include "LightSensor.h"

LightSensor::LightSensor(std::string inputSensorName){
    sensorName = inputSensorName;
    setupSuccessful = false;
    luxFlag = false;
}

void LightSensor::setupLightSensor(){
    if(sensorName == "BH1750"){
        setupSuccessful = setupBH1750();
        luxFlag = true;
    }
}

void LightSensor::readLightSensor(){
    if(!setupSuccessful){
        return;
    }
    if(sensorName == "BH1750"){
        lux = getBH1750lux();
    }
}

void LightSensor::setcsvHeader(){
    csvHeader = "";
    if(luxFlag){
        csvHeader += "LightSensor "; csvHeader += "lux"; csvHeader += ",";
    }
}

String LightSensor::getcsvHeader(){
    return csvHeader;
}

void LightSensor::setdataString(){
    dataString = "";
    if(luxFlag){
        dataString += String(lux); dataString += ",";
    }
}

String LightSensor::getdataString(){
    return dataString;
}