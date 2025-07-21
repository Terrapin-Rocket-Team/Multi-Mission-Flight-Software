#include "Gyro.h"

namespace mmfs
{
    Gyro::Gyro(const char *name) : Sensor("Gyroscope", name)
    {
    }

    Gyro::~Gyro()
    {
    }

    Vector<3> Gyro::getAngVel() const
    {
        return angVel;
    }
} // namespace mmfs