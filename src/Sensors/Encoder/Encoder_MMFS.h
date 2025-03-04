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
        virtual int getSteps() const;
        virtual const char *getTypeString() const override;
        virtual const SensorType getType() const override;
        virtual void update() override;
        virtual bool begin(bool useBiasCorrection = true) override;
        virtual void setInitialSteps(int step);

    protected:
        Encoder_MMFS();
        int currentRelativeSteps;
        int initialSteps;
    };
}
#endif // ENCODER_MMFS_H
