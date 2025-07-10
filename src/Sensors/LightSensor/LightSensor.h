#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

#include "../Sensor.h"

namespace mmfs
{
    class LightSensor : public Sensor
    {
    public:
        virtual ~LightSensor();
        virtual const double getLux() const;

    protected:
        LightSensor(const char *name = "LightSensor");
        double lux;
        double initialLux;
    };
}

#endif // LIGHT_SENSOR_H
