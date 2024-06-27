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

    public:
        BMP280();
        virtual bool initialize() override;
        virtual void update() override;
    };
} // namespace mmfs

#endif