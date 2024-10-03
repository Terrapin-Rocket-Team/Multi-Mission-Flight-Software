#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cstdint>

// ------------------- Number lengths -------------------

// the number of digits in a number, used for string representations of values. These represent a potential "worst case" scenario.

constexpr int MAX_DIGITS_INT = 10; // 2,147,483,647
constexpr int MAX_DIGITS_LONG = 19; // 9,223,372,036,854,775,807
constexpr int MAX_DIGITS_FLOAT = 12; // up to 999,999,999.99
constexpr int MAX_DIGITS_LAT_LON = 12; // 180.0000000

// ------------------------------------------------------

// ------------------ Sensor Constants ------------------

extern const int BUZZER_PIN;
extern const int BUILTIN_LED_PIN;
constexpr double MEAN_SEA_LEVEL_PRESSURE_HPA = 1013.25;
extern const int SENSOR_BIAS_CORRECTION_DATA_LENGTH; // in seconds
extern const int SENSOR_BIAS_CORRECTION_DATA_IGNORE; // in seconds (how many seconds to ignore the most recent data for bias correction)

// ------------------------------------------------------

// -------- Kalman Filter Constants (Unused) ------------

// extern const int NUM_INPUTS;
// extern const int NUM_MEASUREMENTS;
// extern const int NUM_STATES;

// ------------------------------------------------------

// ------------------ State Constants -------------------

extern const int UPDATE_RATE; // in hz
extern const int UPDATE_INTERVAL; // in ms

// ------------------------------------------------------

// ------------------ File Constants --------------------

inline int MAX_FILE_NAME_SIZE = 64; // in characters (including null terminator)
inline uintptr_t PSRAM_STARTING_ADDRESS = 0x70000000;
inline int PSRAM_CLUSTER_SIZE = 1024; // in bytes
inline int ONE_MB = 1048576; // in bytes
inline int MAX_PSRAM_FILES = 5; // in files, defaulted low to save memory

// ------------------------------------------------------

#endif // CONSTANTS_H