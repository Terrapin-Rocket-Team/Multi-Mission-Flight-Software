
#include "MS5611F.h"

using namespace mmfs;

mmfs::MS5611::MS5611(const char *name, uint8_t addr, TwoWire *bus) : Barometer(name), ms(addr, bus) {}

mmfs::MS5611::MS5611(uint8_t addr, TwoWire *bus) : Barometer("MS5611"), ms(addr, bus) {}

bool mmfs::MS5611::init()
{
    if (!ms.begin())
    {
        printf("Failed to initialize MS5611 sensor\n");
        return initialized = false;
    }
    ms.setOversampling(OSR_ULTRA_HIGH);

    return initialized = true;
}
bool mmfs::MS5611::read()
{
    ms.read();
    temp = ms.getTemperature();
    pressure = ms.getPressure();
    return true;
}
