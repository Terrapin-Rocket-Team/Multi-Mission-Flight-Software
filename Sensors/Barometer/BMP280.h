#ifndef BMP280_H
#define BMP280_H

#include <Adafruit_BMP280.h>

static Adafruit_BMP280 bmp; // I2C Interface

extern float GROUNDPRESSURE;
float GROUNDPRESSURE = 1013.25; //in hPa (default sea level pressure)

bool setupBMP280();
double getBMP280pressure();
double getBMP280temperature();
double getBMP280relativeAltitude();


#endif