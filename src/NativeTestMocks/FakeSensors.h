#include "../../src/Sensors/Baro/Barometer.h"
#include "../../src/Sensors/GPS/GPS.h"
#include "../../src/Sensors/IMU/IMU.h"

using namespace mmfs;

class FakeBarometer : public Barometer
{
public:
    FakeBarometer() : Barometer() { init = true;}
    ~FakeBarometer() {}

    void update() override
    {
        // Do nothing
    }
    void update(double p, double t)
    {
        pressure = p;
        temp = t;
    }

    bool initialize() override
    {
        return init;
    }
    bool init;
};

class FakeGPS : public GPS
{
public:
    FakeGPS() : GPS() { init = true;}
    ~FakeGPS() {}

    void update() override
    {
        // Do nothing
    }
    void update(double lat, double lon, double alt)
    {
        this.la = lat;
        longitude = lon;
        altitude = alt;
    }

    bool initialize() override
    {
        return init;
    }
    bool init;

    void setHasFirstFix(bool fix)
    {
        hasFirstFix = fix;
    }
};