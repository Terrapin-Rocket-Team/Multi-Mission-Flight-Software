#ifndef LIS3MDL_H
#define LIS3MDL_H

#include <LIS3MDL.h>
#include <Wire.h>
#include "Mag.h"

namespace mmfs
{
    class LIS3MDL : public mmfs::Mag
    {
    public:
        bool read() override;
        bool init() override;
        LIS3MDL(const char *name = "LIS3MDL", TwoWire &bus = Wire, uint8_t addr = 0x1c);
        LIS3MDL(TwoWire &bus, uint8_t addr = 0x1c);

    protected:
        ::LIS3MDL magmtr;
        // Biases and ranges, calculated by calling the calibration script
        ::LIS3MDL::vector<int16_t> m_min = {-32767, -32767, -32767};
        ::LIS3MDL::vector<int16_t> m_max = {+32767, +32767, +32767};
    };
}
#endif // LIS3MDL_H