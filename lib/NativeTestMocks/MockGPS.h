//
// Created by ramykaddouri on 11/1/24.
//

#ifndef MOCKGPS_H
#define MOCKGPS_H
#include <LaunchDataReader.h>
#include "../../src/Sensors/GPS/GPS.h"


class MockGPS : public mmfs::GPS {
private:
    LaunchDataReader dataReader;

    int posXColIdx = -1;
    int posYColIdx = -1;
    int posZColIdx = -1;
    int headingColIdx = -1;
    int fixQualityColIdx = -1;

    const std::string posXColName;
    const std::string posYColName;
    const std::string posZColName;
    const std::string headingColName;
    const std::string fixQualityColName;

    float launchData[MAX_NUM_COLS]{0.0f};
public:
    MockGPS(const std::filesystem::path& dataPath,
        const std::string& posXColName,
        const std::string& posYColName,
        const std::string& posZColName,
        const std::string& headingColName,
        const std::string& fixQualityColName);
    ~MockGPS() override;

    bool init() override;
    void read() override;

    mmfs::Vector<3> getPos() const override;
    double getHeading() const override;
    int getFixQual() const override;
};



#endif //MOCKGPS_H
