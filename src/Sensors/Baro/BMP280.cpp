#include "BMP280.h"

namespace mmfs
{

    BMP280::BMP280(const char *name, uint8_t addr, TwoWire *wire) : bmp(wire)
    {
        setName(name);
        this->addr = addr;
    }

    BMP280::BMP280(uint8_t addr, TwoWire *wire) : bmp(wire), addr(addr)
    {
        setName("BMP280");
        this->addr = addr;
    }

    bool BMP280::init()
    {
        if (!bmp.begin(addr))
        { // hardware I2C mode, can pass in address & alt Wire
            // Serial.println("Could not find a valid BMP390 sensor, check wiring!");
            return initialized = false;
        }
        return initialized = true;
    }

    void BMP280::read()
    {
        pressure = bmp.readPressure() / 100.0;       // hPa
        temp = bmp.readTemperature();                // C
    }
}