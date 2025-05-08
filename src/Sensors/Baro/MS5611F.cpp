
#include "MS5611F.h"

using namespace mmfs;

mmfs::MS5611::MS5611(const char *name, TwoWire *bus, uint8_t addr) : ms(addr, bus)
{
    setName(name);
}

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
void mmfs::MS5611::read()
{
    ms.read();
    temp = ms.getTemperature();
    pressure = ms.getPressure();
}
