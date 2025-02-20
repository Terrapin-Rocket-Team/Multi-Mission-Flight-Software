#include "GPS.h"

namespace mmfs
{

#pragma region GPS Specific Functions

    GPS::GPS()
    {
        setUpPackedData();
        hr = 0;
        min = 0;
        sec = 0;
        strcpy(tod, "00:00:00");
    }

    GPS::~GPS() {}

    Vector<3> GPS::getPos() const { return position; }

    double GPS::getHeading() const { return heading; }

    Vector<3> GPS::getDisplacement() const { return displacement; }

    Vector<3> GPS::getOrigin() const { return origin; }

    bool GPS::getHasFirstFix() const { return hasFirstFix; }

    int GPS::getFixQual() const { return fixQual; }

    // Taken from this article and repo. As I understand it, it's an accurate approximation of the Vincenty formulae to find the distance between two points on the earth
    //  https://github.com/mapbox/cheap-ruler/blob/main/index.js#L475
    //  https://blog.mapbox.com/fast-geodesic-approximations-with-cheap-ruler-106f229ad016
    void GPS::calcInitialValuesForDistance()
    {
        constexpr auto EARTH_RAD = 6378.137e3; // meters
        constexpr auto RAD = 3.14159265358979323846 / 180.0;

        constexpr auto EARTH_FLAT = 1.0 / 298.257223563; // flattening of the earth. IDK what this means

        constexpr auto ECC_SQRD = EARTH_FLAT * (2.0 - EARTH_FLAT); // eccentricity squared. IDK what this means

        constexpr auto m = RAD * EARTH_RAD;
        const auto coslat = cos(position.x() * RAD);
        const auto w2 = 1.0 / (1.0 - ECC_SQRD * (1.0 - coslat * coslat)); // IDK what this means
        const auto w = sqrt(w2);                                          // IDK what this means

        ky = m * w * coslat;                // IDK what this means
        kx = m * w * w2 * (1.0 - ECC_SQRD); // IDK what this means
    }

    void GPS::calcDistance()
    {
        double dy = wrapLongitude(position.y() - origin.y()) * ky;
        double dx = (position.x() - origin.x()) * kx;
        displacement.x() = dx;
        displacement.y() = dy;
    }

    double GPS::wrapLongitude(double val)
    {
        while (val > 180)
            val -= 360;
        while (val < -180)
            val += 360;
        return val;
    }

    const char *GPS::getTimeOfDay() const
    {
        return tod;
    }

    void GPS::findTimeZone()
    {
        bool isDST = (month > 3 && month < 11) || (month == 3 && day >= 10) || (month == 11 && day < 3);
        if (isDST)
        {
            hrOffset = 1;
            logger.recordLogData(INFO_, "DST is in effect.");
        }
        else
        {
            hrOffset = 0;
            logger.recordLogData(INFO_, "DST is not in effect.");
        }

        if (getPos().x() > -82.5)
        {
            hrOffset -= 5;
            logger.recordLogData(INFO_, "Timezone: Eastern Standard Time");
        }
        else if (getPos().x() > -97.5)
        {
            hrOffset -= 6;
            logger.recordLogData(INFO_, "Timezone: Central Standard Time");
        }
        else if (getPos().x() > -112.5)
        {
            hrOffset -= 7;
            logger.recordLogData(INFO_, "Timezone: Mountain Standard Time");
        }
        else if (getPos().x() > -127.5)
        {
            hrOffset -= 8;
            logger.recordLogData(INFO_, "Timezone: Pacific Standard Time");
        }
        else if (getPos().x() > -135)
        {
            hrOffset -= 9;
            logger.recordLogData(INFO_, "Timezone: Alaska Standard Time");
        }
        else if (getPos().x() > -150)
        {
            hrOffset -= 10;
            logger.recordLogData(INFO_, "Timezone: Hawaii-Aleutian Standard Time");
        }
        else
        {
            logger.recordLogData(INFO_, "Timezone: UTC");
        }
        char log[100];
        snprintf(log, 100, "Timezone offset: %d", hrOffset);
        logger.recordLogData(INFO_, log);
    }

#pragma endregion // GPS Specific Functions

#pragma region Sensor Virtual Function Implementations

    const char *GPS::getTypeString() const { return "GPS"; }

    const SensorType GPS::getType() const { return GPS_; }

    void GPS::update()
    {
        read();

        if (!hasFirstFix && fixQual >= 3)
        {
            logger.recordLogData(INFO_, "GPS has first fix.");
            findTimeZone();

            bb.aonoff(BUZZER_PIN, 1000);
            hasFirstFix = true;
            origin.x() = position.x();
            origin.y() = position.y();
            origin.z() = position.z();

            calcInitialValuesForDistance();
        }
        if (hasFirstFix)
        {
            if (biasCorrectionMode)
            {
                originBuffer.push(position);
                Vector<3> sum = Vector<3>(0, 0, 0);
                int valsToCount = std::min(originBuffer.getCount(), CIRC_BUFFER_LENGTH - CIRC_BUFFER_IGNORE);
                for (int i = 0; i < valsToCount; i++)
                {
                    sum += originBuffer[i];
                }
                origin = sum / valsToCount / 1.0;
            }
            calcDistance();
            displacement.z() = (position.z() - origin.z());

            hr += hrOffset;
            hr = (hr % 24 + 24) % 24; // in cpp -1 % 24 = -1, but we want it to be 23
            min = min % 60;
            sec = sec % 60; // just in case
            snprintf(tod, 12, "%02d:%02d:%02d", hr, min, sec); // size is really 9 but 12 ignores warnings about truncation. IRL it will never truncate
        }
        packData();
    }

    bool GPS::begin(bool useBiasCorrection)
    {
        biasCorrectionMode = useBiasCorrection;
        position = Vector<3>(0, 0, 0);
        displacement = Vector<3>(0, 0, 0);
        origin = Vector<3>(0, 0, 0);
        originBuffer.clear();
        fixQual = 0;
        hasFirstFix = false;
        heading = 0;
        return init();
    }

#pragma region Packed Data Functions

    const int GPS::getNumPackedDataPoints() const { return 8; }

    const PackedType *GPS::getPackedOrder() const
    {
        static const PackedType order[] = {DOUBLE, DOUBLE, FLOAT, FLOAT, FLOAT, FLOAT, BYTE, STRING_10};
        return order;
    }
    const char **GPS::getPackedDataLabels() const
    {
        static const char *labels[] = {"Lat", "Lon", "Alt (m)", "Disp X (m)", "Disp Y (m)", "Disp Z (m)", "Fix Quality", "Time of Day"};
        return labels;
    }
    void GPS::packData()
    {
        float posz = float(position.z());
        float dispx = float(displacement.x());
        float dispy = float(displacement.y());
        float dispz = float(displacement.z());
        int fixQual = int(this->fixQual);
        int offset = 0;
        memcpy(packedData + offset, &position.x(), sizeof(double));
        offset += sizeof(double);
        memcpy(packedData + offset, &position.y(), sizeof(double));
        offset += sizeof(double);
        memcpy(packedData + offset, &posz, sizeof(float));
        offset += sizeof(float);
        memcpy(packedData + offset, &dispx, sizeof(float));
        offset += sizeof(float);
        memcpy(packedData + offset, &dispy, sizeof(float));
        offset += sizeof(float);
        memcpy(packedData + offset, &dispz, sizeof(float));
        offset += sizeof(float);
        memcpy(packedData + offset, &fixQual, PackedTypeToSize(BYTE));
        offset += PackedTypeToSize(BYTE);
        memcpy(packedData + offset, tod, PackedTypeToSize(STRING_10));
    }

    // const char *GPS::getStaticDataString() const
    // {
    //     sprintf(staticData, "Original Latitude (m): %.10f\nOriginal Longitude (m): %.10f\nOriginal Altitude (m): %.2f\n", origin.x(), origin.y(), origin.z());
    //     return staticData;
    // }

#pragma endregion // Packed Data Functions

#pragma endregion // Sensor Virtual Function Implementations
}