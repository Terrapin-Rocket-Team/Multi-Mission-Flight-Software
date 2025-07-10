#include "Accel.h"

namespace mmfs
{
    Accel::Accel(const char *name) : Sensor(name)
    {
    }

    Accel::~Accel()
    {
    }

    Vector<3> Accel::getAccel() const
    {
        return acc;
    }
} // namespace mmfs