#ifndef ENCODER_H
#define ENCODER_H

#include "../Sensor.h"
#include "../../Constants.h"

namespace mmfs
{
    class Encoder : Sensor
    {
    public:
        virtual ~Encoder();
        virtual const char *getTypeString() const override;
        virtual const SensorType getType() const override;
        virtual void update() override;
        virtual bool begin(bool useBiasCorrection = true) override;
        virtual const int getNumPackedDataPoints() const override;
        virtual const PackedType *getPackedOrder() const override;
        virtual const char **getPackedDataLabels() const override;
        virtual void packData();

    protected:
        Encoder();
        int currentRelativeSteps;
        int initialSteps;
    };
}
#endif // ENCODER_H
