#ifndef BMP280_H
#define BMP280_H

#include <Adafruit_BMP280.h>
#include "Barometer.h"

namespace mmfs
{
    class BMP280 : public Barometer
    {
    private:
        Adafruit_BMP280 bmp;
        uint8_t addr;

    public:
        BMP280(const char *name = "BMP280", uint8_t addr = BMP280_ADDRESS, TwoWire *wire = &Wire);
        virtual ~BMP280() {}
        BMP280(uint8_t addr = BMP280_ADDRESS, TwoWire *wire = &Wire);
        virtual bool init() override;
        virtual void read() override;
    };
} // namespace mmfs

#endif