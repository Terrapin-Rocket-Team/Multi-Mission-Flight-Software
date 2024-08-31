#include <Arduino.h>
#include "AvionicsState.h"
#include "../src/BlinkBuzz/BlinkBuzz.h"
#include "../src/Sensors/GPS/MAX_M10S.h"
#include "../src/Sensors/IMU/BNO055.h"
#include "../src/Sensors/Baro/BMP390.h"
#include "../src/Filters/AvionicsKF.h"

using namespace mmfs;
PSRAM *ram;
const int BUZZER_PIN = 33;
const int BUILTIN_LED_PIN = LED_BUILTIN;
int allowedPins[] = {BUILTIN_LED_PIN, BUZZER_PIN};
BlinkBuzz bb(allowedPins, 2, true);
void setup()
{
    MAX_M10S gps;
    BNO055 imu;
    BMP390 baro;
    Sensor *sensors[3] = {&gps, &imu, &baro};
    AvionicsKF kfilter;
    AvionicsState avionicsState(sensors, 3, &kfilter, false);

    ram = new PSRAM();
    if (!setupSDCard())
        bb.onoff(BUZZER_PIN, 200, 3);
    else
        bb.onoff(BUZZER_PIN, 1000, 1);

    if (!ram->init())
        bb.onoff(BUZZER_PIN, 200, 3);
    else
        bb.onoff(BUZZER_PIN, 1000, 1);
    
    if(!avionicsState.init())
        bb.onoff(BUZZER_PIN, 200, 3);
    else
        bb.onoff(BUZZER_PIN, 1000, 1);
}

void loop()
{
    bb.update();
}