#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

#include "../Sensor.h"

namespace mmfs
{
    class LightSensor : public Sensor
    {
    public:
        virtual ~LightSensor() {}
        virtual const char *getCsvHeader() const override;
        virtual const char *getDataString() const override;
        virtual const char *getStaticDataString() const override;
        virtual const double getLux() const;
        virtual const SensorType getType() const override { return LIGHT_SENSOR_; }
        virtual const char *getTypeString() const override { return "Light Sensor"; }
        virtual void update() override;

        virtual const int getNumPackedDataPoints() const override { return 1; }
        virtual const PackedType *getPackingOrder() const override
        {
            static const PackedType order[] = {FLOAT};
            return order;
        }

    protected:
        LightSensor()
        {
            staticData = new char[25 + MAX_DIGITS_FLOAT * 1]; // 25 chars for the string, 12 chars for the float
            data = new char[MAX_DIGITS_FLOAT * 1 + 1];        // 12 chars for the 1 float, 1 for the comma
            setUpPackedData();
        } // Protected constructor to prevent instantiation
        double lux;
        double initialLux;
    };
}
#endif