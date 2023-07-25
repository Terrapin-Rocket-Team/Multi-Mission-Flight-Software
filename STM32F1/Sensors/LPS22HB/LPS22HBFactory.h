//
// Created by kking on 7/24/2023.
//

#ifndef MULTIMISSIONLIBRARY_LPS22HBFACTORY_H
#define MULTIMISSIONLIBRARY_LPS22HBFACTORY_H
#include "LPS22HB.h"
#include "Barometer.h"
class LPS22HBFactory {
public:
    static Barometer* createLps22hb(uint8_t address, I2C_HandleTypeDef* i2c_config, LPS22HB::ODR_CONFIG odr_config, LPS22HB::FIFO_CONFIG fifo_config);
};
#endif //MULTIMISSIONLIBRARY_LPS22HBFACTORY_H
