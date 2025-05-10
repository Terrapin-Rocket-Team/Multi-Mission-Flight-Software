#include "Encoder_MMFS.h"

namespace mmfs
{

#pragma region Encoder Specific Functions

    Encoder_MMFS::~Encoder_MMFS() {}

    Encoder_MMFS::Encoder_MMFS()
    {
        addColumn(INT, &currentRelativeSteps, "Rel Steps");
    }

    int Encoder_MMFS::getSteps() const { return currentRelativeSteps; }

#pragma endregion // Encoder Specific Functions

#pragma region Sensor Virtual Function Implementations

    void Encoder_MMFS::update()
    {
        read();
    }

    bool Encoder_MMFS::begin(bool useBiasCorrection)
    {
        biasCorrectionMode = useBiasCorrection;
        return init();
    }

    void Encoder_MMFS::setInitialSteps(int step)
    {
        initialSteps = step;
        getLogger().recordLogData(INFO_, 100, "[Encoder]: Initial Steps set to: %d", step);
    }

#pragma region Data Reporting

    const char *Encoder_MMFS::getTypeString() const { return "Encoder"; }

    const SensorType Encoder_MMFS::getType() const { return "Encoder"_i; }

#pragma endregion // Data Reporting

#pragma endregion // Sensor Virtual Function Implementations
}