#ifndef ARDUINO_H
#define ARDUINO_H
#include <cstdint>
#include <cstdio>
#include <string.h>
#include <chrono>

#define SS 10 // random ass numbers lol

#define HIGH 1
#define LOW 0

#define INPUT 1
#define OUTPUT 0

#define LED_BUILTIN 13



const auto start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

inline unsigned long millis()
{
    return (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - start);
}

inline void delay(unsigned long ms) {}

inline void digitalWrite(int pin, int value) {}

class SerialClass {
    public:
        void write(const char *data) {}
        void print(const char *data) {}
        void println(const char *data) {}
};

extern SerialClass Serial;

#endif