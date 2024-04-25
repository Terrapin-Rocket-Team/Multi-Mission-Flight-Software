#include "BMP280.h"

bool BMP280::initialize()
{
    if (!bmp.begin())
    { // hardware I2C mode, can pass in address & alt Wire
        // Serial.println("Could not find a valid BMP390 sensor, check wiring!");
        return initialized = false;
    }

    delay(1000);

    double startPressure = 0;
    for (int i = 0; i < 10; i++)
    {
        bmp.readPressure();
        delay(25);
    }
    for (int i = 0; i < 100; i++)
    {
        startPressure += bmp.readPressure();
        delay(25);
    }
    groundPressure = (startPressure / 100.0) / 100.0; // hPa
    return initialized = true;
}

void BMP280::update()
{
    pressure = bmp.readPressure() / 100.0;       // hPa
    temp = bmp.readTemperature();                // C
    altitude = bmp.readAltitude(groundPressure); // m
}
const char *BMP280::getName() const
{
    return "BMP280";
}