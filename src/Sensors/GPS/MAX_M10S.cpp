#include "MAX_M10S.h"
#include "../../BlinkBuzz/BlinkBuzz.h"

namespace mmfs
{

    MAX_M10S::MAX_M10S() : m10s()
    {
        setName("MAX_M10S");
    }

    bool MAX_M10S::initialize()
    {

        // myGNSS.enableDebugging(); // Uncomment this line to enable helpful debug messages on Serial

        int count = 0;
        while (m10s.begin() == false && count < 3) // Connect to the u-blox module using Wire port
        {
            // Serial.println(F("u-blox GNSS not detected at default I2C address. Retrying..."));
            delay(1000);
            count++;
        }
        if (!m10s.begin())
            return initialized = false;

        m10s.setI2COutput(COM_TYPE_UBX);            // Set the I2C port to output UBX only (turn off NMEA noise)
        m10s.setNavigationFrequency(10);            // Set the update rate to 10Hz
        m10s.setDynamicModel(DYN_MODEL_AIRBORNE4g); // Set the dynamic model to airborne with 4g acceleration
        m10s.setAutoPVT(true);                      // Enable automatic PVT reports
        m10s.saveConfiguration();                   // Save the current settings to flash and BBR
        return initialized = true;
    }

    /*
    used to update all instance variables
    */
    void MAX_M10S::update()
    {
        if (!initialized || !m10s.getPVT() || m10s.getInvalidLlh())
            return; // See if new data is available
        double timeLast = time;
        time = millis() / 1000.0;
        pos.x() = m10s.getLatitude() / 10000000.0;
        pos.y() = m10s.getLongitude() / 10000000.0;

        heading = m10s.getHeading();
        altitude = m10s.getAltitude() / 1000.0;
        fixQual = m10s.getSIV();
        if (!hasFirstFix && fixQual >= 3)
        {
            recordLogData(INFO, "GPS has first fix.");

            bb.aonoff(BUZZER_PIN, 1000);
            hasFirstFix = true;
            origin.x() = pos.x();
            origin.y() = pos.y();
            origin.z() = altitude;
        }

        // updated before displacement and gps as the old values and new values are needed to get a
        // significant of a velocity
        velocity.x() = (((pos.x() - origin.x()) * 111319.0) - displacement.x()) / (time - timeLast);
        velocity.y() = (((pos.y() - origin.y()) * 111319.0 * cos(pos.x() * PI / 180.0)) - displacement.y()) / (time - timeLast);
        velocity.z() = ((altitude)-displacement.z()) / (time - timeLast);

        displacement.x() = (pos.x() - origin.x()) * 111319.0;
        displacement.y() = (pos.y() - origin.y()) * 111319.0 * cos(pos.x() * PI / 180.0);
        displacement.z() = (altitude - origin.z());

        hr = m10s.getHour();
        min = m10s.getMinute();
        sec = m10s.getSecond();
// ignore -Wformat-truncation
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-truncation"
        snprintf(gpsTime, 9, "%02d:%02d:%02d", hr, min, sec);
#pragma GCC diagnostic pop
    }
}