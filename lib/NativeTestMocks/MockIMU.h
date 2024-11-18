//
// Created by ramykaddouri on 11/1/24.
//

#ifndef MOCKIMU_H
#define MOCKIMU_H
#include <LaunchDataReader.h>
#include "../../src/Sensors/IMU/IMU.h"


class MockIMU : public mmfs::IMU {
private:
    LaunchDataReader dataReader;

    //Arrays in form: [X, Y, Z]
    std::string accColNames[3];
    std::string gyroColNames[3];
    std::string magColNames[3];

    int accIndices[3];
    int gyroIndices[3];
    int magIndices[3];

    float launchData[MAX_NUM_COLS]{0.0f};
public:
    MockIMU(const std::filesystem::path& dataPath,
        const std::string accColNames[3],
        const std::string gyroColNames[3],
        const std::string magColNames[3]);
    ~MockIMU() override;

    bool init() override;
    void read() override;
};



#endif //MOCKIMU_H
