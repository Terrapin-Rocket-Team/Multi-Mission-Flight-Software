#ifndef DPS310_H
#define DPS310_H
#include <Adafruit_DPS310.h>

#include <Sensors/Baro/Barometer.h>
#include <RecordData/Logging/Logger.h>

namespace mmfs
{
    class DPS310 : public Barometer
    {
    public:
        DPS310(const char *name = "DPS310", uint8_t addr = 0x77, TwoWire *bus = &Wire);
        DPS310(uint8_t addr, TwoWire *bus = &Wire);
        virtual bool init() override;
        virtual void read() override;

    private:
        TwoWire *bus;
        uint8_t addr;
        Adafruit_DPS310 dps;
    };
} // namespace mmfs

#endif // DPS310_H