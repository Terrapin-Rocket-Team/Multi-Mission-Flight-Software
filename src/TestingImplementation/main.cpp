#include <Arduino.h>
#include "AvionicsState.h"
#include "../BlinkBuzz/BlinkBuzz.h"
#include "../Sensors/GPS/MAX_M10S.h"
#include "../Sensors/IMU/BNO055.h"
#include "../Sensors/Baro/BMP390.h"

using namespace mmfs;
PSRAM *ram;
int BUZZER_PIN = 33;
int BUILTIN_LED_PIN = LED_BUILTIN;
int allowedPins[] = {BUILTIN_LED_PIN, BUZZER_PIN};
BlinkBuzz bb(allowedPins, 2, true);
void setup()
{
    MAX_M10S gps;
    BNO055 imu;
    BMP390 baro;
    Sensor *sensors[3] = {&gps, &imu, &baro};
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