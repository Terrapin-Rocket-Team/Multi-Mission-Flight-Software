#include "BMP390.h"


bool BMP390::initialize()
{
    if (!bmp.begin_I2C())
    { // hardware I2C mode, can pass in address & alt Wire
        // Serial.println("Could not find a valid BMP390 sensor, check wiring!");
        return initialized = false;
    }

    delay(1000);

    // Set up oversampling and filter initialization
    bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
    bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
    bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
    bmp.setOutputDataRate(BMP3_ODR_50_HZ);

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

void BMP390::update()
{
    pressure = bmp.readPressure() / 100.0;       // hPa
    temp = bmp.readTemperature();                // C
    altitude = bmp.readAltitude(groundPressure); // m
}
const char *BMP390::getName() const
{
    return "BMP390";
}