#include "../../src/Sensors/Baro/Barometer.h"
#include "../../src/Sensors/GPS/GPS.h"
#include "../../src/Sensors/IMU/IMU.h"

using namespace mmfs;

class FakeBarometer : public Barometer
{
public:
    FakeBarometer() : Barometer() { initialized = true; setName("FakeBarometer"); }
    ~FakeBarometer() {}

    void read() override { pressure = fakeP; temp = fakeT; }
    void set(double p, double t)
    {
        pressure = fakeP = p;
        temp = fakeT = t;
    }

    bool init() override
    {
        return initialized;
    }
    double fakeP = 0;
    double fakeT = 0;

    bool isInBiasCorrectionMode()
    {
        return biasCorrectionMode;
    }
    int getMaxBufferLen()
    {
        return pressureBuffer.getSize();
    }
};

class FakeGPS : public GPS
{
public:
    FakeGPS() : GPS() { initialized = true; setName("FakeGPS"); }
    ~FakeGPS() {}

    void read() override {}
    void set(double lat, double lon, double alt)
    {
        pos.x() = lat;
        pos.y() = lon;
        pos.z() = alt;
    }

    bool init() override
    {
        return initialized;
    }

    void setHasFirstFix(bool fix)
    {
        hasFirstFix = fix;
    }
    void setFixQual(int qual)
    {
        fixQual = qual;
    }
};

class FakeIMU : public IMU
{
public:
    FakeIMU() : IMU() { initialized = true; setName("FakeIMU"); }
    ~FakeIMU() {}

    void read() override { }
    void set(Vector<3> acc, Vector<3> mag, Vector<3> gyro)
    {
        measuredAcc = acc;
        measuredMag = mag;
        measuredGyro = gyro;
    }

    bool init() override
    {
        return initialized;
    }
};