#include "BH1750personal.h"

bool setupBH1750(){
    if(!lightMeter.begin()){
        Serial.println("BH1750 failed to begin");
        return false;
    }
    return true;
}

float getBH1750lux(){
    return lightMeter.readLightLevel();
}

