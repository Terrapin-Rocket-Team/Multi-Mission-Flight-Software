//
// Created by ramykaddouri on 11/1/24.
// Modified by michael mallamaci on 2/28/25 for reading from SD card
//

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

        String accColNames[3];
        String gyroColNames[3];
        String magColNames[3];

        float sdData[MAX_NUM_COLS]{0.0f};

    public:
        MockIMU(const char* dataPath, const String accColNames[3], const String gyroColNames[3], const String magColNames[3]);

        bool init() override;
        void read() override;
    };
}

#endif // MOCKIMU_H
