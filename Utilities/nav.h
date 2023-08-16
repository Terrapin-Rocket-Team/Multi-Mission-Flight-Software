#ifndef NAV_H
#define NAV_H

#include <PWMServo.h>
<<<<<<< Updated upstream
#include "BNO55.cpp"
=======
#include "BNO055.cpp"
>>>>>>> Stashed changes
#include "imumaths.h"

void setupServo();
void moveServo(double p1, double p2, double p3, double p4);
imu::Vector<3> quatToXYZ();
void goDirection(string D, double neg);

#endif
