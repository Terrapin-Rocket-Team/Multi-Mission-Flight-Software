//
// Created by kking on 7/24/2023.
//
#include "LPS22HBFactory.h"
Barometer* LPS22HBFactory::createLps22hb(
        uint8_t address,
        I2C_HandleTypeDef* i2c_config,
        LPS22HB::ODR_CONFIG odr_config,
        LPS22HB::FIFO_CONFIG fifo_config)
{
    LPS22HB lps(i2c_config, address);
    lps.init();
    lps.set_odr(odr_config);
    lps.configure_fifo(fifo_config);
    lps.calibrate();
    return &lps;
};