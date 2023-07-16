#include "Barometer.h"

Barometer::Barometer(std::string inputSensorName){
    sensorName = inputSensorName;
    setupSuccessful = false;
    pressureFlag = false; temperatureFlag = false; relativeAltitudeFlag = false;
}

void Barometer::setupBarometer(){
    if(sensorName == "BMP280"){
        setupSuccessful = setupBMP280();
        pressureFlag = true;
        temperatureFlag = true;
        relativeAltitudeFlag = true;
    }
}

void Barometer::readBarometer(){
    if(!setupSuccessful){
        return;
    }
    if(sensorName == "BMP280"){
        pressure = getBMP280pressure();
        temperature = getBMP280temperature();
        relativeAltitude = getBMP280relativeAltitude();
    }
}

void Barometer::setcsvHeader(){
    csvHeader = "";
    if(relativeAltitudeFlag){
        csvHeader += "Bar "; csvHeader += "Altitude"; csvHeader += ",";
    }
    if(pressureFlag){
        csvHeader += "Bar "; csvHeader += "Pressure"; csvHeader += ",";
    }
    if(temperatureFlag){
        csvHeader += "Bar "; csvHeader += "Temperature"; csvHeader += ",";
    }
}

String Barometer::getcsvHeader(){
    return csvHeader;
}

void Barometer::setdataString(){
    dataString = "";
    if(relativeAltitudeFlag){
        dataString += String(relativeAltitude); dataString += ",";
    }
    if(pressureFlag){
        dataString += String(pressure); dataString += ",";
    }
    if(temperatureFlag){
        dataString += String(temperature); dataString += ",";
    }
}

String Barometer::getdataString(){
    return dataString;
}

