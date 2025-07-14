#include "Accel.h"

namespace mmfs
{
    Accel::Accel(const char *name) : Sensor("Accelerometer", name)
    {
        addColumn(DOUBLE, &acc.x(), "Acc X (m/s^2)");
        addColumn(DOUBLE, &acc.y(), "Acc Y (m/s^2)");
        addColumn(DOUBLE, &acc.z(), "Acc Z (m/s^2)");
    }

    Accel::~Accel()
    {
    }

    Vector<3> Accel::getAccel() const
    {
        return acc;
    }
} // namespace mmfs