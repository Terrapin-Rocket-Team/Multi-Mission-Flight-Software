#ifndef ENCODER_MMFS_H
#define ENCODER_MMFS_H

#include "../Sensor.h"
#include "../../Constants.h"

namespace mmfs
{
    class Encoder_MMFS : public Sensor
    {
    public:
        virtual ~Encoder_MMFS();
        virtual double getSteps() const;
        virtual const char *getTypeString() const override;
        virtual const SensorType getType() const override;
        virtual void update() override;
        virtual bool begin(bool useBiasCorrection = true) override;
        virtual const int getNumPackedDataPoints() const override;
        virtual const PackedType *getPackedOrder() const override;
        virtual const char **getPackedDataLabels() const override;
        virtual void packData();

    protected:
        Encoder_MMFS();
        int currentRelativeSteps;
        int initialSteps;
    };
}
#endif // ENCODER_MMFS_H
