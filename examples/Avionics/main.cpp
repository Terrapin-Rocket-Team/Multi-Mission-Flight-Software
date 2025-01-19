#include <Arduino.h>
#include "AvionicsState.h"
#include "AvionicsKF.h"
#include "MMFS.h"

using namespace mmfs;
const int BUZZER_PIN = 33;

MAX_M10S gps;
BMI088andLIS3MDL imu055;
DPS310 baro;
Sensor *sensors[3] = {&gps, &imu055, &baro};
AvionicsKF kfilter;
AvionicsState avionicsState(sensors, 3, &kfilter);

MMFSConfig config = MMFSConfig()
                        .withBBPin(LED_BUILTIN)
                        .withBuzzerPin(BUZZER_PIN)
                        .withState(&avionicsState);

MMFSSystem sys(&config);

int timeOfLastUpdate = 0;

void setup()
{
    Wire.begin();
    sys.init();
}

void loop()
{
    sys.update();
}