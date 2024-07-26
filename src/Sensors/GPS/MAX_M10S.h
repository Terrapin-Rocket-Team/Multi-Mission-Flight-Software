#ifndef MAX_M10S_H
#define MAX_M10S_H

#include <Wire.h>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#include <SparkFun_u-blox_GNSS_v3.h>
#pragma GCC diagnostic pop
#include <Arduino.h>
#include "GPS.h"

namespace mmfs
{

    class MAX_M10S : public GPS
    {
    private:
        SFE_UBLOX_GNSS m10s;

    public:
        MAX_M10S();
        bool init(bool useBiasCorrection = true) override;
        void update() override;
    };
}
#endif // MAX_M10S_H
