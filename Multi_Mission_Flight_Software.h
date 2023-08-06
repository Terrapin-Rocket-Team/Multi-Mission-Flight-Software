#ifndef MULTI_MISSION_FLIGHT_SOFTWARE_H
#define MULTI_MISSION_FLIGHT_SOFTWARE_H

#include <Arduino.h>

// Include all the Sensors
#include "Sensors/Barometer/Barometer.h"
#include "Sensors/Barometer/Barometer.cpp"
#include "Sensors/GPS/GPS.h"
#include "Sensors/GPS/GPS.cpp"
#include "Sensors/IMU/IMU.h"
#include "Sensors/IMU/IMU.cpp"
#include "Sensors/LightSensor/LightSensor.h"
#include "Sensors/LightSensor/LightSensor.cpp"

// Include State
#include "State/State.h"
#include "State/State.cpp"

// Include all the Utilities
#include "Utilities/buzzer.h"
#include "Utilities/buzzer.cpp"
#include "Utilities/recordData.h"
#include "Utilities/recordData.cpp"
#include "Utilities/time.h"
#include "Utilities/time.cpp"

#endif
