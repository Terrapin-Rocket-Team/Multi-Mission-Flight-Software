#include "BMI088Gyro.h"

namespace mmfs
{
    BMI088Gyro::BMI088Gyro(const char *name, TwoWire &bus, uint8_t addr) : Gyro(name), gyro(bus, addr)
    {
        addColumn(DOUBLE, &angVel.x(), "Gyro X (rad/s)");
        addColumn(DOUBLE, &angVel.y(), "Gyro Y (rad/s)");
        addColumn(DOUBLE, &angVel.z(), "Gyro Z (rad/s)");
    }

    BMI088Gyro::BMI088Gyro(TwoWire &bus, uint8_t addr) : Gyro("BMI088"), gyro(bus, addr)
    {
    }

    bool BMI088Gyro::init()
    {
        return gyro.begin() > 0;
    }

    bool BMI088Gyro::read()
    {
        gyro.readSensor();
        angVel = Vector<3>(gyro.getGyroX_rads(), gyro.getGyroY_rads(), gyro.getGyroZ_rads());
        return true;
    }
} // namespace mmfs