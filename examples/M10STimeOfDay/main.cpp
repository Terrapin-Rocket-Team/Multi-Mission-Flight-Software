#include <Arduino.h>
#include "MMFS.h"

using namespace mmfs;

int timeOfLastUpdate = 0;

MMFSConfig config = MMFSConfig()
                        .withBBPin(LED_BUILTIN)
                        .withBuzzerPin(33);

MAX_M10S gps;

void setup()
{
    Wire.begin();
    Serial.begin(115200);
    Serial.println("M10S Time of Day Example");
    if (gps.begin(true))
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