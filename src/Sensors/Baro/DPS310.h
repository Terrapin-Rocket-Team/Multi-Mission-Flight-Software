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
        DPS310(const char *name = "DPS310", TwoWire *bus = &Wire, uint8_t addr = 119U);
        virtual bool init() override;
        virtual void read() override;

    private:
        TwoWire *bus;
        uint8_t addr;
        Adafruit_DPS310 dps;
    };
}; // namespace mmfs

#endif // DPS310_H