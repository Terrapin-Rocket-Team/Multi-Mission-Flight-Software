#ifndef APRS_TELEM_MSG_H
#define APRS_TELEM_MSG_H

#include "APRSMsgBase.h"
#include <imumaths.h>

const uint8_t PI_ON = 0b00000001;          // Pi is on
const uint8_t PI_VIDEO = 0b00000010;       // Pi is recording video
const uint8_t RECORDING_DATA = 0b00000100; // FC is recording data

struct APRSTelemData
{
    double lat;
    double lng;
    int alt;
    int spd;
    int hdg;
    int stage;
    imu::Vector<3> orientation;
    uint8_t statusFlags;
};

class APRSTelemMsg : public APRSMsgBase
{
public:
    APRSTelemMsg(APRSConfig header);
    APRSTelemData data;
    char *type = "PositionWithoutTimestamp";

private:
    void decodeData(int cursor) override;
    void encodeData(int cursor) override;

    // Scale factors for encoding/decoding ignoring lat/long
    const double ALT_SCALE = (pow(91, 3) / 36000.0);       // (91^3/36000) scale to fit in 3 base91 characters
    const double SPD_SCALE = (pow(91, 2) / 1000.0);        // (91^2/1000) scale to fit in 2 base91 characters
    const double HDG_SCALE = (pow(91, 2) / 360.0);         // (91^2/360) scale to fit in 2 base91 characters
    const double ORIENTATION_SCALE = (pow(91, 2) / 360.0); // same as course

    const int ALT_OFFSET = +1000; // range of -1000 to 35000 ft.
};

#endif // APRS_TELEM_MSG_H