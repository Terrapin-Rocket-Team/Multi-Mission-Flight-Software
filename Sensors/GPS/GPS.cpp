#include "GPS.h"

GPS::GPS(){
    sensorName = "";
    setupSuccessful = false;
    gpsTimer = millis();

    latitudeFlag = false; longitudeFlag = false; altitudeFlag = false;
    gpsPositionFlag = false; headingAngleFlag = false; satellitesFlag = false;
}

GPS::GPS(std::string inputSensorName){
    sensorName = inputSensorName;
    setupSuccessful = false;
    gpsTimer = millis();

    latitudeFlag = false; longitudeFlag = false; altitudeFlag = false;
    gpsPositionFlag = false; headingAngleFlag = false; satellitesFlag = false;
}

void GPS::setupGPS(){
    if(sensorName == "NEO-M9N"){
        setupSuccessful = setupNEOM9N();
        latitudeFlag = true; longitudeFlag = true; altitudeFlag = true;
        gpsPositionFlag = true; headingAngleFlag = true; satellitesFlag = true;
    }
}

void GPS::readGPS(){
    if(!setupSuccessful){
        return;
    }
    if (millis() - gpsTimer > 1000){
        //Query module only every second. Doing it more often will just cause I2C traffic.
        //The module only responds when a new position is available

        gpsTimer = millis(); //Update the timer

        if(sensorName == "NEO-M9N"){
            runNEOM9Nbackground();
            longitude = getNEOM9Nlongitude();
            latitude = getNEOM9Nlatitude();
            altitude = getNEOM9Naltitude();
            gpsPosition = getNEOM9NgpsPosition(longitude, latitude, altitude);
            headingAngle = getNEOM9NheadingAngle();
            satellites = getNEOM9Nsatellites();
        }
    }
}

void GPS::setcsvHeader(){
    csvHeader = "";
    if(longitudeFlag){
        csvHeader += "GPS "; csvHeader += "Longitude"; csvHeader += ",";
    }
    if(latitudeFlag){
        csvHeader += "GPS "; csvHeader += "Latitude"; csvHeader += ",";
    }
    if(altitudeFlag){
        csvHeader += "GPS "; csvHeader += "Altitude"; csvHeader += ",";
    }
    if(gpsPositionFlag){
        csvHeader += "GPS "; csvHeader += "PosX"; csvHeader += ",";
        csvHeader += "GPS "; csvHeader += "PosY"; csvHeader += ",";
        csvHeader += "GPS "; csvHeader += "PosZ"; csvHeader += ",";
    }
    if(headingAngleFlag){
        csvHeader += "GPS "; csvHeader += "Heading Angle"; csvHeader += ",";
    }
    if(satellitesFlag){
        csvHeader += "GPS "; csvHeader += "Satellites"; csvHeader += ",";
    }
}

String GPS::getcsvHeader(){
    return csvHeader;
}

void GPS::setdataString(){
    dataString = "";
    if(longitudeFlag){
        dataString += String(longitude); dataString += ",";
    }
    if(latitudeFlag){
        dataString += String(latitude); dataString += ",";
    }
    if(altitudeFlag){
        dataString += String(altitude); dataString += ",";
    }
    if(gpsPositionFlag){
        dataString += String(gpsPosition.x()); dataString += ",";
        dataString += String(gpsPosition.y()); dataString += ",";
        dataString += String(gpsPosition.z()); dataString += ",";
    }
    if(headingAngleFlag){
        dataString += String(headingAngle); dataString += ",";
    }
    if(satellitesFlag){
        dataString += String(satellites); dataString += ",";
    }
}

String GPS::getdataString(){
    return dataString;
}