#include "LightSensor.h"

namespace mmfs
{
#pragma region LightSensor Specific Functions

    LightSensor::LightSensor() : lux(0), initialLux(0), Sensor("Light Sensor")
    {
        addColumn(DOUBLE, &lux, "Lux (Lux)");
    }

    LightSensor::~LightSensor() {}

    const double LightSensor::getLux() const { return lux; }

#pragma endregion // LightSensor Specific Functions

#pragma region Sensor Virtual Function Implementations

    void LightSensor::update()
    {
        read();
    }

    bool LightSensor::begin(bool useBiasCorrection)
    {
        return init();
    }

#pragma endregion // Sensor Virtual Function Implementations
}