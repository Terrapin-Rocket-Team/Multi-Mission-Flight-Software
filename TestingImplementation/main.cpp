#include <Arduino.h>
#include "AvionicsState.h"
#include <BlinkBuzz.h>
#include <Sensors/GPS/MAX_M10S.h>
#include <Sensors/IMU/BNO055.h>
#include <Sensors/Baro/BMP390.h>

PSRAM *ram;
const int BUZZER = 33;
int allowedPins[] = {LED_BUILTIN, BUZZER};
BlinkBuzz bb(allowedPins, 2, true);
void setup()
{

    KalmanInterface kfilter(3, 3, 6);
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
    
    avionicsState.addSensor(new MAX_M10S());
    avionicsState.addSensor(new BNO055());
    avionicsState.addSensor(new BMP390());
    if(!avionicsState.init())
        bb.onoff(BUZZER, 200, 3);
    else
        bb.onoff(BUZZER, 1000, 1);
}

void loop()
{
    bb.update();
}