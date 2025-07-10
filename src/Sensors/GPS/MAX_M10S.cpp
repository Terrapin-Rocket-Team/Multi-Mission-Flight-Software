#include "MAX_M10S.h"
#include "../../BlinkBuzz/BlinkBuzz.h"

namespace mmfs
{

    MAX_M10S::MAX_M10S(const char *name, TwoWire *wire, uint8_t address) : GPS(name), m10s(), wire(wire), address(address)
    {

    }

    MAX_M10S::MAX_M10S(TwoWire *wire, uint8_t address) : GPS("MAX-M10S"), m10s(), wire(wire), address(address)
    {

    }

    bool MAX_M10S::init()
    {
        // m10s.enableDebugging(); // Uncomment this line to enable helpful debug messages on Serial

        int count = 0;
        while (m10s.begin(*wire, address) == false && count < 3) // Connect to the u-blox module using Wire port
        {
            // Serial.println(F("u-blox GNSS not detected at default I2C address. Retrying..."));
            delay(250);
            count++;
        }
        if (!m10s.begin(*wire, address))
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
    bool MAX_M10S::read()    {
        if (!initialized || !m10s.getPVT() || m10s.getInvalidLlh())
            return false; // See if new data is available

        position.x() = m10s.getLatitude() / 10000000.0;
        position.y() = m10s.getLongitude() / 10000000.0;
        position.z() = m10s.getAltitude() / 1000.0;
        heading = m10s.getHeading();
        fixQual = m10s.getSIV();
        hr = m10s.getHour();
        min = m10s.getMinute();
        sec = m10s.getSecond();
        day = m10s.getDay();
        month = m10s.getMonth();
        year = m10s.getYear();
        return true;
    }
}