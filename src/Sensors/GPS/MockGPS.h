#ifndef MOCKGPS_H
#define MOCKGPS_H

#include "../SerialReader.h"
#include "GPS.h"

namespace mmfs
{
class MockGPS : public mmfs::GPS {
private:
    SerialReader dataReader;

    int posXColIdx = -1;
    int posYColIdx = -1;
    int posZColIdx = -1;
    int headingColIdx = -1;
    int fixQualityColIdx = -1;

    std::string posXColName;
    std::string posYColName;
    std::string posZColName;
    std::string headingColName;
    std::string fixQualityColName;

    float sdData[MAX_NUM_COLS]{0.0f};
public:
    MockGPS(const char* dataPath,
            const std::string& posXColName,
            const std::string& posYColName,
            const std::string& posZColName,
            const std::string& headingColName,
            const std::string& fixQualityColName);

    bool init() override;
    void read() override;
};

}

#endif // MOCKGPS_H
