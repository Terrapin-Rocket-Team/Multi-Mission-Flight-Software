#include <Arduino.h>
#include "MMFS.h"

using namespace mmfs;


const int BUZZER_PIN = 33;
const int BUILTIN_LED_PIN = LED_BUILTIN;
int allowedPins[] = {BUILTIN_LED_PIN, BUZZER_PIN};
BlinkBuzz bb(allowedPins, 2, true);

Logger logger;
PSRAM *psram;
ErrorHandler errorHandler;

const int UPDATE_RATE = 10;
const int UPDATE_INTERVAL = 1000.0 / UPDATE_RATE;

 int timeOfLastUpdate = 0;


MAX_M10S gps;

void setup()
{
    Wire.begin();
    Serial.begin(115200);
    Serial.println("M10S Time of Day Example");
    if(gps.begin(true))
        Serial.println("GPS initialized.");
    else
        Serial.println("GPS failed to initialize.");
}

void loop()
{
    gps.update();
    Serial.print("Time of Day: ");
    Serial.print(gps.getTimeOfDay());
    Serial.println(" Fix Quality: " + String(gps.getFixQual()));
    delay(1000);
}