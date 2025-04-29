#ifndef NATIVE_TEST_HELPER_H
#define NATIVE_TEST_HELPER_H

#include <Arduino.h>
#include "../../src/Constants.h"
#include "../../src/BlinkBuzz/BlinkBuzz.h"
#include "../../src/RecordData/Logging/Logger.h"
#include <string>
#ifdef PIO_UNIT_TESTING
#define UNITY_INCLUDE_DOUBLE
#include <unity.h>
#endif // PIO_UNIT_TESTING


#endif // NATIVE_TEST_HELPER_H
