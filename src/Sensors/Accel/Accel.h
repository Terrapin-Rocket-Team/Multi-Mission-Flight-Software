#ifndef ACCEL_H
#define ACCEL_H

#include "../Sensor.h"
#include "../../Math/Vector.h"

namespace mmfs
{
    class Accel : public Sensor
    {
    public:
        virtual ~Accel();
        virtual Vector<3> getAccel() const;

    protected:
        Accel(const char *name = "Accelerometer");
        Vector<3> acc = Vector<3>(0, 0, 0);
    };
}
#endif // ACCEL_H