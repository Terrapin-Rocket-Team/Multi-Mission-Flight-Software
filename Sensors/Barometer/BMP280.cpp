#include "BMP280.h"

float GROUNDPRESSURE = 1013.25; //in hPa (default sea level pressure)

bool setupBMP280(){
    if(!bmp.begin()){
        Serial.println("BMP280 failed to begin");
        return false;
    }
    float totalPressure = 0;
    for(int i = 0; i < 10; i++){
        totalPressure += bmp.readPressure();
        delay(100);
    }
    GROUNDPRESSURE = totalPressure/10;
    GROUNDPRESSURE = GROUNDPRESSURE/100;  //conversion from Pa to hPa
    return true;
}

double getBMP280pressure(){
    return bmp.readPressure();
}

double getBMP280temperature(){
    return bmp.readTemperature();
}

double getBMP280relativeAltitude(){
    return bmp.readAltitude(GROUNDPRESSURE);
}