#include <Arduino.h>
#include "AvionicsState.h"
#include "AvionicsKF.h"
#include "MMFS.h"
#include "RetrieveData/SerialHandler.h"

using namespace mmfs;
const int BUZZER_PIN = 33;

MAX_M10S gps;
BMI088andLIS3MDL mmfsimu;
DPS310 baro;
Sensor *sensors[3] = {&gps, &mmfsimu, &baro};
AvionicsKF kfilter;
AvionicsState avionicsState(sensors, 3, &kfilter);

MMFSConfig config = MMFSConfig()
                        .withBBPin(LED_BUILTIN)
                        .withBuzzerPin(BUZZER_PIN)
                        .withState(&avionicsState);

MMFSSystem sys(&config);


SerialHandler sh;

void setup()
{
    Serial.println("Starting up");
    sys.init();
}

void loop()
{
    if(Serial.available()){
        sh.handle();
    }
    sys.update();
}