#include <Arduino.h>
#include "AvionicsState.h"
#include <BlinkBuzz.h>
#include "../Sensors/GPS/MAX_M10S.h"
#include "../Sensors/IMU/BNO055.h"
#include "../Sensors/Baro/BMP390.h"

PSRAM *ram;
const int BUZZER = 33;
int allowedPins[] = {LED_BUILTIN, BUZZER};
BlinkBuzz bb(allowedPins, 2, true);
void setup()
{
    Sensor *sensors[3] = {new MAX_M10S(), new BNO055(), new BMP390()};
    KalmanInterface kfilter(3, 3, 6);
    AvionicsState avionicsState(sensors, 3, &kfilter, false);

    ram = new PSRAM();
    if (!setupSDCard())
        bb.onoff(BUZZER, 200, 3);
    else
        bb.onoff(BUZZER, 1000, 1);

    if (!ram->init())
        bb.onoff(BUZZER, 200, 3);
    else
        bb.onoff(BUZZER, 1000, 1);
    
    if(!avionicsState.init())
        bb.onoff(BUZZER, 200, 3);
    else
        bb.onoff(BUZZER, 1000, 1);
}

void loop()
{
    bb.update();
}