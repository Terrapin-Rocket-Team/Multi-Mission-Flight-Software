#include "BNO055.h"

using namespace mmfs;

BNO055::BNO055() : bno()
{
    setName("BNO055");
}
bool BNO055::initialize()
{
    if (!bno.begin())
    {
        return initialized = false;
    }
    bno.setExtCrystalUse(true);

    initialMagField = bno.getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER);
    return initialized = true;
}

void BNO055::update()
{
    orientation = bno.getQuat();
    accelerationVec = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
    orientationEuler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
    magnetometer = bno.getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER);
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
