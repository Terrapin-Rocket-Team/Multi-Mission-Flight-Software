#include "../NativeTestMocks/Arduino.h"
#include "../../src/RecordData/RecordData.h"

SerialClass Serial;
PSRAM pram;
PSRAM *ram = &pram;
uint8_t external_psram_size = 0;