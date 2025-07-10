#include "Mag.h"

namespace mmfs
{
    Mag::Mag(const char *name) : Sensor("Magnetometer", name)
    {
    }

    Mag::~Mag()
    {
    }

    Vector<3> Mag::getMag() const
    {
        return mag;
    }
} // namespace mmfs