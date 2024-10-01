#ifndef NATIVE_TEST_HELPER_H
#define NATIVE_TEST_HELPER_H

#include <Arduino.h>
#include "../../src/Constants.h"
#include "../../src/BlinkBuzz/BlinkBuzz.h"
#include "../../src/RecordData/Logger.h"
#include <string>
#include <unity.h>

#define UNITY_INCLUDE_DOUBLE

extern int allowed_pins[];
extern BlinkBuzz bb;
extern mmfs::Logger logger;

#endif // NATIVE_TEST_HELPER_H
