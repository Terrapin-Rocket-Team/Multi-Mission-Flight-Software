//
// Created by ramykaddouri on 10/29/24.
//

#ifndef MOCKBAROMETER_H
#define MOCKBAROMETER_H
#include "LaunchDataReader.h"
#include "../../src/Sensors/Baro/Barometer.h"

class MockBarometer : public mmfs::Barometer {
private:
    LaunchDataReader dataReader;

    int pressureColIndex = -1;
    int temperatureColIndex = -1;

    const std::string pressureColName;
    const std::string temperatureColName;

    float launchData[MAX_NUM_COLS]{0.0f};
public:
    MockBarometer(const std::filesystem::path& dataPath, const std::string& pressureColName, const std::string& temperatureColName);
    ~MockBarometer() override;

    bool init() override;
    void read() override;
};



#endif //MOCKBAROMETER_H
