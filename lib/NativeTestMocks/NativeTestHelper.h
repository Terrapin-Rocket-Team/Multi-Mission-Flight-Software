#ifndef NATIVE_TEST_HELPER_H
#define NATIVE_TEST_HELPER_H

#include <Arduino.h>
#include "../../src/Constants.h"
#include "../../src/BlinkBuzz/BlinkBuzz.h"
#include "../../src/RecordData/Logger.h"
#include "../../src/Error/ErrorHandler.h"
#include <string>
#ifdef PIO_UNIT_TESTING
#include <unity.h>
#endif

#define UNITY_INCLUDE_DOUBLE

extern int allowed_pins[];
extern BlinkBuzz bb;
extern mmfs::Logger logger;
extern mmfs::ErrorHandler errorHandler;

#endif // NATIVE_TEST_HELPER_H
