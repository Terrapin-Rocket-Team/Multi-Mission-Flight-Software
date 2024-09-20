#ifndef NATIVE_TEST_HELPER_H
#define NATIVE_TEST_HELPER_H

#include <Arduino.h>
#include "../../src/Constants.h"
#include "../../src/BlinkBuzz/BlinkBuzz.h"
#include <string>
#include <unity.h>

#define UNITY_INCLUDE_DOUBLE

extern int allowed_pins[];
extern BlinkBuzz bb;

extern const int UPDATE_RATE;
extern const int SENSOR_BIAS_CORRECTION_DATA_IGNORE;
extern const int SENSOR_BIAS_CORRECTION_DATA_LENGTH;
extern const int UPDATE_INTERVAL;
extern const int BUZZER_PIN;

#endif // NATIVE_TEST_HELPER_H
