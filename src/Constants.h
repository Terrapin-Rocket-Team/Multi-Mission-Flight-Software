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
inline unsigned int SENSOR_BIAS_CORRECTION_DATA_LENGTH = 2; // in seconds
inline unsigned int SENSOR_BIAS_CORRECTION_DATA_IGNORE = 1; // in seconds (how many seconds to ignore the most recent data for bias correction)

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
constexpr unsigned int MAX_FILES = 10;
inline unsigned int ONE_MB = 1048576; // in bytes

// ------------------------------------------------------

#endif // CONSTANTS_H