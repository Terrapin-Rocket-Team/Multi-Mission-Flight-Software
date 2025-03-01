//
// Created by ramykaddouri on 11/1/24.
// Modified by michael mallamaci on 3/1/25 for reading from SD card
//

#ifndef MOCKGPS_H
#define MOCKGPS_H

#include "../SdDataReader.h"
#include "GPS.h"

namespace mmfs
{
class MockGPS : public mmfs::GPS {
private:
    SdDataReader dataReader;

    int posXColIdx = -1;
    int posYColIdx = -1;
    int posZColIdx = -1;
    int headingColIdx = -1;
    int fixQualityColIdx = -1;

    String posXColName;
    String posYColName;
    String posZColName;
    String headingColName;
    String fixQualityColName;

    float sdData[MAX_NUM_COLS]{0.0f};
public:
    MockGPS(const char* dataPath,
            const String& posXColName,
            const String& posYColName,
            const String& posZColName,
            const String& headingColName,
            const String& fixQualityColName);
    ~MockGPS() override;

    bool init() override;
    void read() override;

    mmfs::Vector<3> getPos() const override;
    double getHeading() const override;
    int getFixQual() const override;
};

}

#endif // MOCKGPS_H
