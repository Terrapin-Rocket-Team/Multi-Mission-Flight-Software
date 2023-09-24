#include "NEO-M9N.h"

bool gpsFirstFixReceived = false;

bool setupNEOM9N(){

    Wire.begin();

    //myGNSS.enableDebugging(); // Uncomment this line to enable helpful debug messages on Serial

    if(!myGNSS.begin()){  //Connect to the u-blox module using Wire port
        Serial.println(F("u-blox GNSS not detected at default I2C address. Please check wiring."));
        return false;
    }

    myGNSS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
    myGNSS.setNavigationFrequency(5);
    myGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); //Save (only) the communications port settings to flash and BBR
    return true;
}

void runNEOM9Nbackground(){
    fixQuality = (int)myGNSS.getFixType();

    if (!gpsFirstFixReceived && fixQuality != 0) {
        gpsStartPos = imu::Vector<3>(getNEOM9Nlongitude(), getNEOM9Nlongitude(), getNEOM9Naltitude());
        gpsFirstFixReceived = true;
    }
}

double getNEOM9Nlongitude(){
    return (double)myGNSS.getLongitude() / 10000000.0;
}

double getNEOM9Nlatitude(){
    return (double)myGNSS.getLatitude() / 10000000.0;
}

double getNEOM9Naltitude(){
    return (double)myGNSS.getAltitude() / 1000.0;
}

imu::Vector<3> getNEOM9NgpsPosition(double longitude, double latitude, double altitude){
    // Serial.print("*");
    // https://stackoverflow.com/questions/15736995/how-can-i-quickly-estimate-the-distance-between-two-latitude-longitude-points
    // Gives vector in meters

    double lon1 = gpsStartPos.x();
    double lat1 = gpsStartPos.y();

    double R = 6371;  // radius of the earth in km
    double x = (radians(longitude) - radians(lon1)) * cos(0.5 * (radians(latitude) + radians(lat1)));
    double y = radians(latitude) - radians(lat1);
    double d = R * sqrt(x*x + y*y) * 1000;

    double deltaLatNorm = (latitude - lat1) / (sqrt((latitude - lat1) * (latitude - lat1) + (longitude - lon1) * (longitude - lon1)));
    double deltaLonNorm = (longitude - lon1) / (sqrt((latitude - lat1) * (latitude - lat1) + (longitude - lon1) * (longitude - lon1)));  

    imu::Vector<2> movement = imu::Vector<2>(d * deltaLonNorm, d * deltaLatNorm);

    //update position vector from start position   
    return imu::Vector<3>(movement.x(), movement.y(), altitude - gpsStartPos.z());
}

double getNEOM9NheadingAngle(){
    return (double)myGNSS.getHeading() / 100000;
}

int getNEOM9Nsatellites(){
    return (int)myGNSS.getSIV();
}