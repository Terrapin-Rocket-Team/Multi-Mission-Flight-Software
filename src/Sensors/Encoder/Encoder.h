#ifndef ENCODER_H
#define ENCODER_H

#include "../Sensor.h"
#include "../../Constants.h"

namespace mmfs
{
    class Encoder : Sensor
    {

    public:
        virtual ~Encoder() {}
        virtual const char *getCsvHeader() const override;
        virtual const char *getDataString() const override;
        virtual const char *getStaticDataString() const override;
        virtual const char *getTypeString() const override { return "Encoder"; }
        virtual const SensorType getType() const override { return ENCODER_; }
        virtual void update() override;
        virtual bool begin(bool useBiasCorrection = true) override;

        virtual const int getNumPackedDataPoints() const override { return 1; }
        virtual const PackedType *getPackingOrder() const override
        {
            static const PackedType order[] = {INT};
            return order;
        }
        virtual void packData();

    protected:
        Encoder()
        {                                       // Protected constructor to prevent instantiation
            staticData = new char[25 + MAX_DIGITS_FLOAT * 1]; // 25 chars for the string, 12 chars for the float
            data = new char[MAX_DIGITS_FLOAT * 1 + 3];        // 12 chars for the float, 3 for the comma
            setUpPackedData();
        }
        int currentRelativeSteps;
        int initialSteps;
    };
}
#endif // ENCODER_H