//
// Created by ramykaddouri on 10/29/24.
// Modified by michael mallamaci on 2/26/25 for reading from SD card
//

#ifndef MOCKBAROMETER_H
#define MOCKBAROMETER_H
#include "../SdDataReader.h"
#include "Barometer.h"

using namespace mmfs;

class MockBarometer : public mmfs::Barometer {
    private:
        SdDataReader dataReader;

        int pressureColIndex = -1;
        int temperatureColIndex = -1;

        String pressureColName;
        String temperatureColName;

        float sdData[MAX_NUM_COLS]{0.0f};
    public:
        MockBarometer(const char* dataPath, const String& pressureColName, const String& temperatureColName);
        ~MockBarometer() override;

        bool init() override;
        void read() override;
    };

#endif //MOCKBAROMETER_H