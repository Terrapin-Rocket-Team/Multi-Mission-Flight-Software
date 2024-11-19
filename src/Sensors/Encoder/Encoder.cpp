#include "Encoder.h"

namespace mmfs
{

#pragma region Encoder Specific Functions

    Encoder::~Encoder() {}

    Encoder::Encoder()
    {
        setUpPackedData();
        // Additional constructor logic
    }

#pragma endregion // Encoder Specific Functions

#pragma region Sensor Virtual Function Implementations

    void Encoder::update()
    {
        read();
        packData();
    }

    bool Encoder::begin(bool useBiasCorrection)
    {
        biasCorrectionMode = useBiasCorrection;
        return init();
    }

#pragma region Data Reporting

    const char *Encoder::getTypeString() const { return "Encoder"; }

    const SensorType Encoder::getType() const { return ENCODER_; }

    const int Encoder::getNumPackedDataPoints() const { return 1; }

    const PackedType *Encoder::getPackedOrder() const
    {
        static const PackedType order[] = {INT};
        return order;
    }

    const char **Encoder::getPackedDataLabels() const
    {
        static const char *labels[] = {"Rel Steps"};
        return labels;
    }

    void Encoder::packData()
    {
        int offset = 0;
        memcpy(packedData + offset, &currentRelativeSteps, sizeof(int));
    }

    // const char *Encoder::getStaticDataString() const
    // {
    //     sprintf(staticData, "Initial Steps: %d\n", initialSteps);
    //     return staticData;
    // }

#pragma endregion // Data Reporting

#pragma endregion // Sensor Virtual Function Implementations
}