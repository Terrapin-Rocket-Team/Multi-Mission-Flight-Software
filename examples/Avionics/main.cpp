#include <Arduino.h>
#include "AvionicsState.h"
#include "AvionicsKF.h"
#include "MMFS.h"

using namespace mmfs;
const int BUZZER_PIN = 33;
const int BUILTIN_LED_PIN = LED_BUILTIN;
int allowedPins[] = {BUILTIN_LED_PIN, BUZZER_PIN};
BlinkBuzz bb(allowedPins, 2, true);


MAX_M10S gps;
BMI088andLIS3MDL imu055;
DPS310 baro;
Sensor *sensors[3] = {&gps, &imu055, &baro};
AvionicsKF kfilter;
Logger logger;
AvionicsState avionicsState(sensors, 3, &kfilter);
PSRAM *psram;
ErrorHandler errorHandler;

const int UPDATE_RATE = 10;
const int UPDATE_INTERVAL = 1000.0 / UPDATE_RATE;

int timeOfLastUpdate = 0;

void setup()
{
    Wire.begin();
    //If you need to change these values, do that here (in setup, before anything else)
    //SENSOR_BIAS_CORRECTION_DATA_LENGTH = 2;
    //SENSOR_BIAS_CORRECTION_DATA_IGNORE = 1;
    psram = new PSRAM();
    logger.init(&avionicsState);


    if (!(logger.isSdCardReady()))
        bb.onoff(BUZZER_PIN, 200, 3);
    else
        bb.onoff(BUZZER_PIN, 1000, 1);

    if (!(logger.isPsramReady()))
        bb.onoff(BUZZER_PIN, 200, 3);
    else
        bb.onoff(BUZZER_PIN, 1000, 1);
    
    if(!avionicsState.init(true))
        bb.onoff(BUZZER_PIN, 200, 3);
    else
        bb.onoff(BUZZER_PIN, 1000, 1);
    logger.writeCsvHeader();
}

void loop()
{
    //Do this as often as possible for best results
    bb.update();
    
    //Do this at a fixed rate
    int currentTime = millis();
    if (currentTime - timeOfLastUpdate < UPDATE_INTERVAL)
        return;
    timeOfLastUpdate = currentTime;

    
    avionicsState.updateState();
    logger.recordFlightData();
}