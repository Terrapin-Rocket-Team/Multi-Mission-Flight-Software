#include "Arduino.h"


const uint64_t start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

uint64_t millis()
{
    return (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - start);
}

#ifndef WIN32
void Sleep(long ms) {}
#endif

void delay(unsigned long ms) { Sleep(ms); }

void delay(int ms) { Sleep(ms); }

void digitalWrite(int pin, int value)
{

    int color;
    switch (pin)
    {
    case 13:
        color = 36;
        break;
    case 33:
        color = 33;
        break;
    case 32:
        color = 95;
        break;
    default:
        color = 0;
        break;
    }
    printf("\x1B[%dm%.3f - %d to \x1B[%dm%s\x1B[0m\n", color, millis() / 1000.0, pin, value == LOW ? 91 : 92, value == LOW ? "LOW" : "HIGH");
}

void SerialClass::write(const char *data, int i)
{
    while (i > 0)
    {
        fakeBuffer[cursor++] = *data;
        data++;
        i--;
    }
    fakeBuffer[cursor] = '\0';
}

void SerialClass::print(const char *data)
{
    write(data, strlen(data));
    printf("%s", data);
}

void SerialClass::println(const char *data)
{
    print(data);
    print("\n");
    printf("%s\n", data);
}
void SerialClass::printf(const char *data, ...)
{
    va_list args;
    va_start(args, data);
    vprintf(data, args);
    va_end(args);
}

void SerialClass::begin(int baud) {}

void SerialClass::clearBuffer()
{
    cursor = 0;
    fakeBuffer[0] = '\0';
}

SerialClass Serial;