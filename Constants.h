#ifndef CONSTANTS_H
#define CONSTANTS_H

// ------------------- Number lengths -------------------

// the number of digits in a number, used for string representations of values. These represent a potential "worst case" scenario.

constexpr int MAX_DIGITS_INT = 10; // 2,147,483,647
constexpr int MAX_DIGITS_LONG = 19; // 9,223,372,036,854,775,807
constexpr int MAX_DIGITS_FLOAT = 12; // up to 999,999,999.99

// ------------------------------------------------------

// ------------------ Sensor Constants ------------------

constexpr int BUZZER_PIN = 33;
constexpr int BUILTIN_LED_PIN = 13;

// ------------------------------------------------------

#endif // CONSTANTS_H