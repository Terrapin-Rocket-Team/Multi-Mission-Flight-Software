#include "BMI088Accel.h"

namespace mmfs
{
    BMI088Accel::BMI088Accel(const char *name, TwoWire &bus, uint8_t addr) : Accel(name), accel(bus, addr)
    {
    }

    BMI088Accel::BMI088Accel(TwoWire &bus, uint8_t addr) : Accel("BMI088"), accel(bus, addr)
    {
    }

    bool BMI088Accel::init()
    {
        return accel.begin() > 0;
    }

    bool BMI088Accel::read()
    {
        accel.readSensor();
        acc = Vector<3>(accel.getAccelX_mss(), accel.getAccelY_mss(), accel.getAccelZ_mss());
        return true;
    }
} // namespace mmfs