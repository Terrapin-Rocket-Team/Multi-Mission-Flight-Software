#include "BMI088andLIS3MDL.h"

using namespace mmfs;

bool BMI088andLIS3MDL::init()
{
    int accelStatus = accel.begin();
    int gyroStatus = gyro.begin();

    int magStatus = mag.init();
    if (magStatus != 0)
    {
        mag.enableDefault();
    }

    initialized = (accelStatus > 0 && gyroStatus > 0);

    //read all data once to setup complementary filter
    accel.readSensor();
    gyro.readSensor();
    mag.read();
    measuredMag = mmfs::Vector<3>(mag.m.x, mag.m.y, mag.m.z);
    measuredAcc = mmfs::Vector<3>(accel.getAccelX_mss(), accel.getAccelY_mss(), accel.getAccelZ_mss());
    measuredGyro = mmfs::Vector<3>(gyro.getGyroX_rads(), gyro.getGyroY_rads(), gyro.getGyroZ_rads());

    quaternionBasedComplimentaryFilterSetup();
    setAccelBestFilteringAtStatic(.5);
    setMagBestFilteringAtStatic(.5);
    return initialized;
}

void BMI088andLIS3MDL::read()
{
    accel.readSensor();
    gyro.readSensor();
    mag.read();

    measuredMag = mmfs::Vector<3>(mag.m.x, mag.m.y, mag.m.z);
    measuredAcc = mmfs::Vector<3>(accel.getAccelX_mss(), accel.getAccelY_mss(), accel.getAccelZ_mss());
    measuredGyro = mmfs::Vector<3>(gyro.getGyroX_rads(), gyro.getGyroY_rads(), gyro.getGyroZ_rads());
    
    quaternionBasedComplimentaryFilter(UPDATE_INTERVAL / 1000.0);

}


