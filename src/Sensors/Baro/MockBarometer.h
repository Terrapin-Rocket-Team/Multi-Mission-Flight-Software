//
// Created by ramykaddouri on 10/29/24.
// Modified by michael mallamaci on 2/26/25 for reading from SD card
//

#ifndef MOCKBAROMETER_H
#define MOCKBAROMETER_H
#include "../SdDataReader.h"
#include "Barometer.h"

class MockBarometer : public mmfs::Barometer {
private:
    LaunchDataReader dataReader;

    int pressureColIndex = -1;
    int temperatureColIndex = -1;

    String pressureColName;
    String temperatureColName;

    float launchData[MAX_NUM_COLS]{0.0f};
public:
    MockBarometer(const std::filesystem::path& dataPath, const String& pressureColName, const String& temperatureColName);
    ~MockBarometer() override;

    bool init() override;
    void read() override;
};



#endif //MOCKBAROMETER_H