#ifndef GPS_H
#define GPS_H

#include <string>

#include "NEO-M9N.h"
#include "NEO-M9N.cpp"


class GPS {
    //Code written for ["NEO-M9N"]
public:
    std::string sensorName;
    bool setupSuccessful;
    double longitude;
    double latitude;
    double altitude;
    imu::Vector<3> gpsPosition;  //in meters from the start location
    double headingAngle;    //degrees from true north/heading
    int satellites;   //number of satellites in use
    uint32_t gpsTimer;

    GPS();
    GPS(std::string inputSensorName);
    void setupGPS();
    void readGPS();

    void setcsvHeader();
    String getcsvHeader();
    void setdataString();
    String getdataString();

private:
    bool longitudeFlag;
    bool latitudeFlag;
    bool altitudeFlag;
    bool gpsPositionFlag;
    bool headingAngleFlag;
    bool satellitesFlag;

    String csvHeader;
    String dataString;

};

#endif