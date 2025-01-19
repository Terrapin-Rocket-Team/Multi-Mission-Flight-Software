#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cstdint>

#ifndef APP_VERSION
#define APP_VERSION "0.0.0"
#endif

// ------------------- Number lengths -------------------

// the number of digits in a number, used for string representations of values. These represent a potential "worst case" scenario.

constexpr int MAX_DIGITS_INT = 10; // 2,147,483,647
constexpr int MAX_DIGITS_LONG = 19; // 9,223,372,036,854,775,807
constexpr int MAX_DIGITS_FLOAT = 12; // up to 999,999,999.99
constexpr int MAX_DIGITS_LAT_LON = 12; // 180.0000000

// ------------------------------------------------------

// ------------------ Sensor Constants ------------------

constexpr double MEAN_SEA_LEVEL_PRESSURE_HPA = 1013.25;
inline int SENSOR_BIAS_CORRECTION_DATA_LENGTH = 2; // in seconds
inline int SENSOR_BIAS_CORRECTION_DATA_IGNORE = 1; // in seconds (how many seconds to ignore the most recent data for bias correction)

// ------------------------------------------------------

// -------- Kalman Filter Constants (Unused) ------------

// extern const int NUM_INPUTS;
// extern const int NUM_MEASUREMENTS;
// extern const int NUM_STATES;

// ------------------------------------------------------

// ------------------ State Constants -------------------

inline unsigned int UPDATE_RATE = 10; // in hz
inline unsigned int UPDATE_INTERVAL = 100; // in ms

// ------------------------------------------------------

// ------------------ File Constants --------------------

inline unsigned int MAX_FILE_NAME_SIZE = 64; // in characters (including null terminator)
inline uintptr_t PSRAM_STARTING_ADDRESS = 0x70000000;
inline unsigned int PSRAM_CLUSTER_SIZE = 62500; // in bytes (32 KB because we are limited to 256 clusters (TODO))
inline unsigned int ONE_MB = 1048576; // in bytes
inline unsigned int MAX_PSRAM_FILES = 5; // in files, defaulted low to save memory

// ------------------------------------------------------

#endif // CONSTANTS_H