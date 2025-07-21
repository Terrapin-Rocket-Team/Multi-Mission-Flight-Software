#ifndef MAG_H
#define MAG_H

#include "../Sensor.h"
#include "../../Math/Vector.h"

namespace mmfs
{
    class Mag : public Sensor
    {
    public:
        virtual ~Mag();
        virtual Vector<3> getMag() const;

    protected:
        Mag(const char *name = "Magnetometer");
        Vector<3> mag = Vector<3>(0, 0, 0);
    };
}
#endif // MAG_H