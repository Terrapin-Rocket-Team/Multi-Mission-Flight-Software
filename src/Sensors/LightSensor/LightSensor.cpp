#include "LightSensor.h"

namespace mmfs
{
#pragma region LightSensor Specific Functions

    LightSensor::LightSensor() : lux(0), initialLux(0)
    {
        setUpPackedData();
    }

    LightSensor::~LightSensor() {}

    const double LightSensor::getLux() const { return lux; }

#pragma endregion // LightSensor Specific Functions

#pragma region Sensor Virtual Function Implementations

    const SensorType LightSensor::getType() const { return LIGHT_SENSOR_; }

    const char *LightSensor::getTypeString() const { return "Light Sensor"; }

    void LightSensor::update()
    {
        read();
    }

    bool LightSensor::begin(bool useBiasCorrection)
    {
        return init();
    }

#pragma region Data Reporting

    const int LightSensor::getNumPackedDataPoints() const { return 1; }

    const PackedType *LightSensor::getPackedOrder() const
    {
        static const PackedType order[] = {FLOAT};
        return order;
    }

    const char **LightSensor::getPackedDataLabels() const
    {
        static const char *labels[] = {"Lux (Lux)"};
        return labels;
    }

    void LightSensor::packData()
    {
        float l = float(lux);
        memcpy(packedData, &l, sizeof(float));
    }

    // const char *LightSensor::getStaticDataString() const
    // {
    //     sprintf(staticData, "Initial Light (lux): %.2f\n", initialLux);
    //     return staticData;
    // }

#pragma endregion // Data Reporting

#pragma endregion // Sensor Virtual Function Implementations
}