#include <Adafruit_BNO055.h>
#include "IMU.h"

class BNO055 : public IMU
{
private:
    Adafruit_BNO055 bno;

public:
//BNO Returns ACC in m/s^2, orientation in quaternion, orientation in euler angles, and magnetometer in uT (microteslas)

    virtual void calibrateBno();
    virtual bool initialize() override;
    virtual char const *getName() const override;
    virtual void update() override;
};
