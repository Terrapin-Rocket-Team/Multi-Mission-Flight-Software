#ifndef BMP280_H
#define BMP280_H

#include <Adafruit_BMP280.h>
#include "Barometer.h"

class BMP280 : public Barometer
{
private:
    Adafruit_BMP280 bmp;

public:
    virtual bool initialize() override;
    virtual void update() override;
    virtual char const *getName() const override;
};

#endif