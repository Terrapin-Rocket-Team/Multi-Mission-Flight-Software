#ifndef MOCKBAROMETER_H
#define MOCKBAROMETER_H
#include "../SerialReader.h"
#include "Barometer.h"

namespace mmfs
{
class MockBarometer : public mmfs::Barometer {
    private:
        SerialReader dataReader;

        int pressureColIndex = -1;
        int temperatureColIndex = -1;

        std::string pressureColName;
        std::string temperatureColName;

        float sdData[MAX_NUM_COLS]{0.0f};
    public:
        MockBarometer(const char* dataPath, const std::string& pressureColName, const std::string& temperatureColName);

        bool init() override;
        bool read() override;
    };
}

#endif //MOCKBAROMETER_H