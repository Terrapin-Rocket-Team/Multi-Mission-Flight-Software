#include "GPS.h"

namespace mmfs
{

    GPS::GPS()
    {
        staticData = new char[60 + MAX_DIGITS_LAT_LON * 2 + MAX_DIGITS_FLOAT * 1];                    // 60 chars for the string, 15 chars for the 2 floats, 12 chars for the float
        data = new char[MAX_DIGITS_LAT_LON * 2 + MAX_DIGITS_FLOAT * 4 + MAX_DIGITS_INT * 1 + 8]; // 15 chars for the 2 floats, 12 chars for the 4 floats, 10 chars for the time of day string, 10 for the int, 8 for the comma
    }

    /*
    returns the lat and long of the rocket and the altitude
    */
    Vector<3> GPS::getPos() const
    {
        return pos;
    }

    double GPS::getHeading() const
    {
        return heading;
    }

    /*
    return the displacement from the origin in meters
    */
    Vector<3> GPS::getDisplacement() const
    {
        return displacement;
    }

    /*
    returns vector of orginal position in lat(deg), lat(deg), and alti(m)
    */
    Vector<3> GPS::getOrigin() const
    {
        return origin;
    }

    bool GPS::getHasFirstFix() const
    {
        return hasFirstFix;
    }

    /*
    return the number of satellites to indicate quality of data
    */
    int GPS::getFixQual() const
    {
        return fixQual;
    }

    const char *GPS::getCsvHeader() const
    {                                                                                                // incl G- for GPS
        return "G-Lat (deg),G-Lon (deg),G-Alt (m),G-DispX (m),G-DispY (m),G-DispZ (m),G-# of Sats,"; // trailing comma
    }

    const char *GPS::getDataString() const
    {
        sprintf(data, "%.10f,%.10f,%.2f,%.2f,%.2f,%.2f,%d,", pos.x(), pos.y(), pos.z(), displacement.x(), displacement.y(), displacement.z(), fixQual); // trailing comma
        return data;
    }

    const char *GPS::getStaticDataString() const
    {
        sprintf(staticData, "Original Latitude (m): %.10f\nOriginal Longitude (m): %.10f\nOriginal Altitude (m): %.2f\n", origin.x(), origin.y(), origin.z());
        return staticData;
    }

    void GPS::update()
    {
        read();

        if (!hasFirstFix && fixQual >= 3)
        {
            recordLogData(INFO, "GPS has first fix.");

            bb.aonoff(BUZZER_PIN, 1000);
            hasFirstFix = true;
            origin.x() = pos.x();
            origin.y() = pos.y();
            origin.z() = pos.z();
        }
        if (hasFirstFix)
        {
            // flat earth approximation TODO: use haversine formula
            displacement.x() = (pos.x() - origin.x()) * 111319.0;
            displacement.y() = (pos.y() - origin.y()) * 111319.0 * cos(pos.x() * 3.14159265 / 180.0);
            displacement.z() = (pos.z() - origin.z());
        }
    }

    bool GPS::begin(bool useBiasCorrection)
    {
        pos = Vector<3>(0, 0, 0);
        displacement = Vector<3>(0, 0, 0);
        origin = Vector<3>(0, 0, 0);
        fixQual = 0;
        hasFirstFix = false;
        heading = 0;
    }
}