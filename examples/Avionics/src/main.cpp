#include <Arduino.h>
#include "AvionicsState.h"
#include <BlinkBuzz.h>

PSRAM *ram;
const int BUZZER = 33;
int allowedPins[] = {LED_BUILTIN, BUZZER};
BlinkBuzz bb(allowedPins, 2, true);
void setup()
{
    KalmanInterface kfilter(3, 3, 3);
    SensorType t[3] = {GPS_, IMU_, BAROMETER_};
    AvionicsState avionicsState(3, t, &kfilter, false);

    ram = new PSRAM();
    if (!setupSDCard())
        bb.onoff(BUZZER, 200, 3);
    else
        bb.onoff(BUZZER, 1000, 1);

    if (!ram->init())
        bb.onoff(BUZZER, 200, 3);
    else
        bb.onoff(BUZZER, 1000, 1);
    
    //avionicsState.addSensor((), 1);
}

void loop()
{
    bb.update();
    // put your main code here, to run repeatedly:
}