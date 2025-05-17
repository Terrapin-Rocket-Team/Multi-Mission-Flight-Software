#include "../../src/Sensors/Baro/Barometer.h"
#include "../../src/Sensors/GPS/GPS.h"
#include "../../src/Sensors/IMU/IMU.h"

using namespace mmfs;

class FakeBarometer : public Barometer
{
public:
    FakeBarometer() : Barometer()
    {
        initialized = true;
        setName("FakeBarometer");
    }
    ~FakeBarometer() {}

    void read() override
    {
        pressure = fakeP;
        temp = fakeT;
    }
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
    FakeGPS() : GPS()
    {
        initialized = true;
        setName("FakeGPS");
    }
    ~FakeGPS() {}

    void read() override {}
    void set(double lat, double lon, double alt)
    {
        position.x() = lat;
        position.y() = lon;
        position.z() = alt;
    }
    void setDateTime(int y, int m, int d, int h, int mm, int s)
    {
        year = y;
        month = m;
        day = d;
        hr = h;
        min = mm;
        sec = s;
        snprintf(tod, 12, "%02d:%02d:%02d", hr, min, sec); // size is really 9 but 12 ignores warnings about truncation. IRL it will never truncate
    }

    bool init() override
    {
        return initialized;
    }

    void setHasFirstFix(bool fix)
    {
        hasFix = fix;
        if (fix)
            fixQual = 4;
        else
            fixQual = 0;
    }
    void setFixQual(int qual)
    {
        fixQual = qual;
    }
    bool isInBiasCorrectionMode()
    {
        return biasCorrectionMode;
    }

    int getMaxBufferLen()
    {
        return originBuffer.getSize();
    }
};

class FakeIMU : public IMU
{
public:
    FakeIMU() : IMU()
    {
        initialized = true;
        setName("FakeIMU");
    }
    ~FakeIMU() {}

    void read() override
    {
    }
    void set(Vector<3> acc, Vector<3> gyro, Vector<3> mag)
    {
        measuredAcc = acc;
        measuredGyro = gyro;
        measuredMag = mag;
    }

    bool init() override
    {
        measuredAcc = Vector<3>{0, 0, -9.8};
        measuredGyro = Vector<3>{0, 0, 0};
        measuredMag = Vector<3>{0, 0, 0};
        orientation = Quaternion{1, 0, 0, 0};
        return initialized;
    }
};