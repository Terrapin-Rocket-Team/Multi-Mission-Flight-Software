#include "Encoder_MMFS.h"

namespace mmfs
{

#pragma region Encoder Specific Functions

    Encoder_MMFS::~Encoder_MMFS() {}

    Encoder_MMFS::Encoder_MMFS()
    {
        setUpPackedData();
        // Additional constructor logic
    }

    double Encoder_MMFS::getSteps() const { return currentRelativeSteps; }

#pragma endregion // Encoder Specific Functions

#pragma region Sensor Virtual Function Implementations

    void Encoder_MMFS::update()
    {
        read();
        packData();
    }

    bool Encoder_MMFS::begin(bool useBiasCorrection)
    {
        biasCorrectionMode = useBiasCorrection;
        return init();
    }

#pragma region Data Reporting

    const char *Encoder_MMFS::getTypeString() const { return "Encoder"; }

    const SensorType Encoder_MMFS::getType() const { return ENCODER_; }

    const int Encoder_MMFS::getNumPackedDataPoints() const { return 1; }

    const PackedType *Encoder_MMFS::getPackedOrder() const
    {
        static const PackedType order[] = {INT};
        return order;
    }

    const char **Encoder_MMFS::getPackedDataLabels() const
    {
        static const char *labels[] = {"Rel Steps"};
        return labels;
    }

    void Encoder_MMFS::packData()
    {
        int offset = 0;
        memcpy(packedData + offset, &currentRelativeSteps, sizeof(int));
    }

    // const char *Encoder_MMFS::getStaticDataString() const
    // {
    //     sprintf(staticData, "Initial Steps: %d\n", initialSteps);
    //     return staticData;
    // }

#pragma endregion // Data Reporting

#pragma endregion // Sensor Virtual Function Implementations
}