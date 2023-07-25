//
// Created by kking on 7/24/2023.
//
#include "LPS22HBFactory.h"

int main()
{
    Barometer* lps = LPS22HBFactory::createLps22hb(LPS22HB::default_address, &hi2c1, LPS22HB::ODR_CONFIG::ODR_75hz, LPS22HB::FIFO_CONFIG::BYPASS);
    lps->get_rel_alt_ft(); //Get current altitude
    lps->get_pressure(); //Get pressure in HPA
    lps->get_temp(); //Get temp in C
    delete lps; //Free the allocated memory once done
};