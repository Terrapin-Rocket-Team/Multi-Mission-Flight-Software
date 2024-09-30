#include "NativeTestHelper.h"

uint8_t external_psram_size = 0;

int allowed_pins[] = {13, 33};
BlinkBuzz bb(allowed_pins, 2, true);
mmfs::Logger logger;
mmfs::ErrorHandler errorHandler;

const int UPDATE_RATE = 10;
const int SENSOR_BIAS_CORRECTION_DATA_IGNORE = 1;
const int SENSOR_BIAS_CORRECTION_DATA_LENGTH = 2;
const int UPDATE_INTERVAL = 1.0 / UPDATE_RATE * 1000;
const int BUZZER_PIN = 14;
