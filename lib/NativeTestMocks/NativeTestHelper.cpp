#include "NativeTestHelper.h"

extern "C" {
    uint8_t external_psram_size = 0;
}

const int BUZZER_PIN = 14;
const int LED_PIN = 13;
int allowed_pins[] = {BUZZER_PIN, LED_PIN};
BlinkBuzz bb(allowed_pins, 2, true);
mmfs::Logger logger;
mmfs::ErrorHandler errorHandler;

const int UPDATE_RATE = 10;
const int UPDATE_INTERVAL = 1.0 / UPDATE_RATE * 1000;
