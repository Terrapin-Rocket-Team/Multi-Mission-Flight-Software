#include "BMP280.h"

using namespace mmfs;

BMP280::BMP280(const char *name, uint8_t addr, TwoWire *wire) : Barometer(name), addr(addr), bmp(wire)
{
}

BMP280::BMP280(uint8_t addr, TwoWire *wire) : Barometer("BMP280"), addr(addr), bmp(wire)
{
}

bool BMP280::init()
{
    if (!bmp.begin(addr))
    { // hardware I2C mode, can pass in address & alt Wire
        // Serial.println("Could not find a valid BMP280 sensor, check wiring!");
        return initialized = false;
    }
    return initialized = true;
}

bool BMP280::read(){
    pressure = bmp.readPressure() / 100.0; // hPa
    temp = bmp.readTemperature();          // C
    return true;
}