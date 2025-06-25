#ifndef MOCKIMU_H
#define MOCKIMU_H

#include "../SerialReader.h"
#include "IMU.h"

namespace mmfs
{
class MockIMU : public mmfs::IMU {
    private:
        SerialReader dataReader;

        int accIndices[3]{-1, -1, -1};
        int gyroIndices[3]{-1, -1, -1};
        int magIndices[3]{-1, -1, -1};

        std::string accColNames[3];
        std::string gyroColNames[3];
        std::string magColNames[3];

        float sdData[MAX_NUM_COLS]{0.0f};

    public:
        MockIMU(const char* dataPath, const std::string accColNames[3], const std::string gyroColNames[3], const std::string magColNames[3]);

        bool init() override;
        bool read() override;
    };
}

#endif // MOCKIMU_H
