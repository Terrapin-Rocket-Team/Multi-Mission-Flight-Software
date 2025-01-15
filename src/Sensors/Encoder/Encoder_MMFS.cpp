#include "Encoder_MMFS.h"

namespace mmfs
{

#pragma region Encoder Specific Functions

    Encoder_MMFS::~Encoder_MMFS() {}

    Encoder_MMFS::Encoder_MMFS()
    {
        addColumn(INT, &currentRelativeSteps, "Rel Steps");
        // Additional constructor logic
    }

    int Encoder_MMFS::getSteps() const { return currentRelativeSteps; }

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

    // const char *Encoder_MMFS::getStaticDataString() const
    // {
    //     sprintf(staticData, "Initial Steps: %d\n", initialSteps);
    //     return staticData;
    // }

#pragma endregion // Data Reporting

#pragma endregion // Sensor Virtual Function Implementations
}