#include "../NativeTestMocks/Arduino.h"
#include "../../src/RecordData/RecordData.h"
#include "../../src/Constants.h"

SerialClass Serial;
PSRAM pram;
PSRAM *ram = &pram;
uint8_t external_psram_size = 0;

const int UPDATE_RATE = 10;
const int SENSOR_BIAS_CORRECTION_DATA_IGNORE = 1;
const int SENSOR_BIAS_CORRECTION_DATA_LENGTH = 2;
const int UPDATE_INTERVAL = 1.0 / UPDATE_RATE * 1000;