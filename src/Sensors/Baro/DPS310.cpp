#include <Adafruit_DPS310.h>

#include "DPS310.h"

using namespace mmfs;

DPS310::DPS310(const char *name, uint8_t addr, TwoWire *bus) : bus(bus), addr(addr), Barometer(name) { }

DPS310::DPS310(uint8_t addr, TwoWire *bus) : bus(bus), addr(addr) { }

bool DPS310::init()
{
    if (!dps.begin_I2C(addr, bus))
    {
        printf("Failed to initialize DPS310 sensor\n");
        return initialized = false;
    }

    // Set up sampling rate and oversampling
    dps.configurePressure(DPS310_64HZ, DPS310_32SAMPLES);
    dps.configureTemperature(DPS310_64HZ, DPS310_8SAMPLES);

    // Operation mode of the sensor. See section 8.5 of the datasheet.
    dps.setMode(DPS310_CONT_PRESTEMP);

    return initialized = true;
}

void DPS310::read()
{
    sensors_event_t temp_event, pressure_event;

    /* getEvents returns true or false depending on whether the sensors were successfully read or not */
    if (dps.getEvents(&temp_event, &pressure_event))
    {
        this->temp = temp_event.temperature;
        this->pressure = pressure_event.pressure;
    }
    else
    {
        getLogger().recordLogData(ERROR_, "Failed to read data from DPS310 sensor", BOTH);
    }
}
