#ifndef DPS368_H
#define DPS368_H

// Although this says DPS310 and we use the DPS368, the internal workings of the sensors are nearly identical.
// Infineon has their own version of the library that explicitly supports the 368,
// but it seems both harder to use and does not build on Platformio without minor adjustments.
// Thus, we stick with the Adafruit library for now. This could be a project to look into in the future if someone finds it worthwhile.
#include <Adafruit_DPS310.h>

#include <Sensors/Baro/Barometer.h>
#include <RecordData/Logging/Logger.h>

namespace mmfs
{
    class DPS368 : public Barometer
    {
    public:
        DPS368(const char *name = "DPS368", TwoWire *bus = &Wire, uint8_t addr = 0x77); // alternative is 0x76
        virtual bool init() override;
        virtual void read() override;

    private:
        TwoWire *bus;
        uint8_t addr;
        Adafruit_DPS310 dps;
    };
}; // namespace mmfs

#endif // DPS368_H