#include "Arduino.h"
#include "../../src/Constants.h"
#include "../../src/RecordData/RecordData.h"
#include "../../src/BlinkBuzz/BlinkBuzz.h"
#include <string>
#include <unity.h>

#define UNITY_INCLUDE_DOUBLE

PSRAM pram;
PSRAM *ram = &pram;
uint8_t external_psram_size = 0;

int allowed_pins[] = {13, 33};

BlinkBuzz bb(allowed_pins, 2, true);

const int UPDATE_RATE = 10;
const int SENSOR_BIAS_CORRECTION_DATA_IGNORE = 1;
const int SENSOR_BIAS_CORRECTION_DATA_LENGTH = 2;
const int UPDATE_INTERVAL = 1.0 / UPDATE_RATE * 1000;
const int BUZZER_PIN = 14;
