#include "Encoder_MMFS.h"

namespace mmfs
{

#pragma region Encoder Specific Functions

    Encoder_MMFS::~Encoder_MMFS() {}

    Encoder_MMFS::Encoder_MMFS() : Sensor("Encoder")
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

#pragma endregion // Data Reporting

#pragma endregion // Sensor Virtual Function Implementations
}