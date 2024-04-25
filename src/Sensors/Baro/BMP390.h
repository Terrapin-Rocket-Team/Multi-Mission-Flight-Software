#ifndef BMP390_H
#define BMP390_H

#include <Adafruit_BMP3XX.h>
#include "Barometer.h"


class BMP390 : public Barometer
{
private:
    Adafruit_BMP3XX bmp;

public:
    BMP390();
    virtual bool initialize() override;
    virtual void update() override;
};

#endif // BMP390_H
