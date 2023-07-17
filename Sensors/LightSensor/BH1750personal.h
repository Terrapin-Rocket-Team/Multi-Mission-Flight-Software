#ifndef BH1750_PERSONAL_H
#define BH1750_PERSONAL_H

#include <BH1750.h>

static BH1750 lightMeter;

bool setupBH1750();
float getBH1750lux();

#endif