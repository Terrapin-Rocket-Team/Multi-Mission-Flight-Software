#ifndef BMP280_H
#define BMP280_H

#include <Adafruit_BMP280.h>
#include "Barometer.h"

namespace mmfs
{
    class BMP280 : public Barometer
    {
    private:
        uint8_t addr;
        Adafruit_BMP280 bmp;

    public:
        BMP280(const char *name = "BMP280", uint8_t addr = BMP280_ADDRESS, TwoWire *wire = &Wire);
        virtual ~BMP280() {}
        BMP280(uint8_t addr = BMP280_ADDRESS, TwoWire *wire = &Wire);
        virtual bool init() override;
        virtual void read() override;
    };
} // namespace mmfs

#endif