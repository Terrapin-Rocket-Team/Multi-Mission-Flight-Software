#ifndef BMP390_H
#define BMP390_H

#include <Adafruit_BMP3XX.h>
#include "Barometer.h"

namespace mmfs
{
    class BMP390 : public Barometer
    {
    private:
        TwoWire *wire;
        uint8_t addr;
        Adafruit_BMP3XX bmp;

    public:
        BMP390(const char *name = "BMP390", uint8_t address = BMP3XX_DEFAULT_ADDRESS, TwoWire *theWire = &Wire);
        BMP390(uint8_t address, TwoWire *theWire = &Wire);
        virtual bool init() override;
        virtual bool read() override;
    };
}

#endif // BMP390_H
