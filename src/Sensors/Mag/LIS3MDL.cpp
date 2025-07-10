#include "LIS3MDL.h"

namespace mmfs
{
    LIS3MDL::LIS3MDL(const char *name, TwoWire &bus, uint8_t addr) : Mag(name), magmtr()
    {
    }

    LIS3MDL::LIS3MDL(TwoWire &bus, uint8_t addr) : Mag("LIS3MDL"), magmtr()
    {
    }

    bool LIS3MDL::init()
    {
        if(!magmtr.init())
        {
            return false;
        }
        magmtr.enableDefault();
        return true;
    }

    bool LIS3MDL::read()
    {
        magmtr.read();
        // Apply hard iron calibration
        mag = mmfs::Vector<3>(
            ((float)magmtr.m.x - (m_max.x + m_min.x) / 2) / ((m_max.x - m_min.x) / 2),
            ((float)magmtr.m.y - (m_max.y + m_min.y) / 2) / ((m_max.y - m_min.y) / 2),
            ((float)magmtr.m.z - (m_max.z + m_min.z) / 2) / ((m_max.z - m_min.z) / 2));
        return true;
    }
} // namespace mmfs