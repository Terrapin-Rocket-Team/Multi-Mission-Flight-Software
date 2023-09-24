#ifndef NEO_M9N_H
#define NEO_M9N_H

#include <SparkFun_u-blox_GNSS_Arduino_Library.h>
#include "../IMU/imumaths.h"

static SFE_UBLOX_GNSS myGNSS;

extern int fixQuality; // gives the int quality of the most recent fix (0, 1, 2 = Invalid, GPS, DGPS)
extern imu::Vector<3> gpsStartPos;   // in longitude, latitude, altitude
extern bool gpsFirstFixReceived;  //if we've gotten the first fix yet

bool setupNEOM9N();
void runNEOM9Nbackground();
double getNEOM9Nlongitude();
double getNEOM9Nlatitude();
double getNEOM9Naltitude();
imu::Vector<3> getNEOM9NgpsPosition(double longitude, double latitude, double altitude);
double getNEOM9NheadingAngle();
int getNEOM9Nsatellites();

#endif