#ifndef RECORD_DATA_H
#define RECORD_DATA_H

#include <stdexcept>

#include "psram.h"
#include "sdCard.h"
#include "../State/State.h"

int PRE_FLIGHT_DATA_DUMP_DURATION = 600;  //in seconds
int PRE_FLIGHT_TIME_SINCE_LAST_DUMP = 0;  //in seconds
int PRE_FLIGHT_TIME_OF_LAST_DUMP = 0;  //in seconds

String CSV_HEADER;

void recordData(State& state, String stage);  //Stages can be "PreFlight", "Flight", "PostFlight"
void dataToPSRAM(State& state);

#endif