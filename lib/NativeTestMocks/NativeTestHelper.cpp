#include "NativeTestHelper.h"

uint8_t external_psram_size = 0;

int allowed_pins[] = {13, 33};
BlinkBuzz bb(allowed_pins, 2, true);
mmfs::Logger logger;

const int UPDATE_RATE = 10;
const int UPDATE_INTERVAL = 1.0 / UPDATE_RATE * 1000;
const int BUZZER_PIN = 14;
