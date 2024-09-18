#include "BNO055.h"

namespace mmfs
{

    BNO055::BNO055(const char *name) : bno()
    {
        setName(name);
    }
    bool BNO055::init()
    {
        if (!bno.begin())
        {
            return initialized = false;
        }
        bno.setExtCrystalUse(true);

        initialMagField = convertIMUtoMMFS(bno.getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER));
        return initialized = true;
    }

    void BNO055::read()
    {
        measuredAcc = convertIMUtoMMFS(bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL));
        measuredGyro = convertIMUtoMMFS(bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE));
        measuredMag = convertIMUtoMMFS(bno.getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER));
    }

    void BNO055::calibrateBno()
    {
        uint8_t system, gyro, accel, mag, i = 0;
        while ((system != 3) || (gyro != 3) || (accel != 3) || (mag != 3))
        {
            bno.getCalibration(&system, &gyro, &accel, &mag);
            i = i + 1;
            if (i == 10)
            {
                i = 0;
            }
            delay(10);
        }
    }
}