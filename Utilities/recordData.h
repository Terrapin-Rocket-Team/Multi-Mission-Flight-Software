#ifndef RECORD_DATA_H
#define RECORD_DATA_H

#include <stdexcept>

#include "psram.h"
#include "sdCard.h"
#include "../State/State.h"

extern int PRE_FLIGHT_DATA_DUMP_DURATION;
extern int PRE_FLIGHT_TIME_SINCE_LAST_DUMP;
extern int PRE_FLIGHT_TIME_OF_LAST_DUMP;

PRE_FLIGHT_DATA_DUMP_DURATION = 600;  //in seconds
PRE_FLIGHT_TIME_SINCE_LAST_DUMP = 0;  //in seconds
PRE_FLIGHT_TIME_OF_LAST_DUMP = 0;  //in seconds

void recordData(State& state, String stage);  //Stages can be "PreFlight", "Flight", "PostFlight"
void dataToPSRAM(State& state);

#endif