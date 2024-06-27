#ifndef BNO055_H
#define BNO055_H

#include <Adafruit_BNO055.h>
#include "IMU.h"

namespace mmfs
{
    class BNO055 : public IMU
    {
    private:
        Adafruit_BNO055 bno;

    public:
        // BNO Returns ACC in m/s^2, orientation in quaternion, orientation in euler angles, and magnetometer in uT (microteslas)
        BNO055();
        virtual void calibrateBno();
        virtual bool initialize() override;
        virtual void update() override;
    };
}

#endif // BNO055_H