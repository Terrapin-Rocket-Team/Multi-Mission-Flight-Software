#ifndef BMI088GYRO_H
#define BMI088GYRO_H

#include <BMI088.h>
#include "Gyro.h"

namespace mmfs
{
    class BMI088Gyro : public mmfs::Gyro
    {
    public:
        bool read() override;
        bool init() override;
        BMI088Gyro(const char *name = "BMI088", TwoWire &bus = Wire, uint8_t addr = 0x68);
        BMI088Gyro(TwoWire &bus, uint8_t addr = 0x68);
    protected:
        Bmi088Gyro gyro;
    };
}
#endif // BMI088GYRO_H