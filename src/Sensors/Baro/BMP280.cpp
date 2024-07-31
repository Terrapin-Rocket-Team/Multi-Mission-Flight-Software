#include "BMP280.h"

namespace mmfs
{

    BMP280::BMP280(const char *name) : bmp()
    {
        setName(name);
    }

    bool BMP280::init()
    {
        if (!bmp.begin())
        { // hardware I2C mode, can pass in address & alt Wire
            // Serial.println("Could not find a valid BMP390 sensor, check wiring!");
            return initialized = false;
        }

        double startPressure = 0;
        for (int i = 0; i < 10; i++)
        {
            bmp.readPressure();
            delay(25);
        }
        return initialized = true;
    }

    void BMP280::read()
    {
        pressure = bmp.readPressure() / 100.0;       // hPa
        temp = bmp.readTemperature();                // C
    }
}