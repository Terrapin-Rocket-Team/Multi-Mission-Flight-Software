#ifndef BMI088ACCEL_H
#define BMI088ACCEL_H

#include <BMI088.h>
#include "Accel.h"

namespace mmfs
{
    class BMI088Accel : public mmfs::Accel
    {
    public:
        bool read() override;
        bool init() override;
        BMI088Accel(const char *name = "BMI088", TwoWire &bus = Wire, uint8_t addr = 0x18);
        BMI088Accel(TwoWire &bus, uint8_t addr = 0x18);

    protected:
        Bmi088Accel accel;
    };
}
#endif // BMI088ACCEL_H