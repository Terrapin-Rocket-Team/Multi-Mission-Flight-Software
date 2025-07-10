#include "Mag.h"

namespace mmfs
{
    Mag::Mag(const char *name) : Sensor(name)
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