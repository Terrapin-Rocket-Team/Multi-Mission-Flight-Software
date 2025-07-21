#include "Mag.h"

namespace mmfs
{
    Mag::Mag(const char *name) : Sensor("Magnetometer", name)
    {
        addColumn(DOUBLE, &mag.x(), "Mag X (uT)");
        addColumn(DOUBLE, &mag.y(), "Mag Y (uT)");
        addColumn(DOUBLE, &mag.z(), "Mag Z (uT)");
    }

    Mag::~Mag()
    {
    }

    Vector<3> Mag::getMag() const
    {
        return mag;
    }
} // namespace mmfs