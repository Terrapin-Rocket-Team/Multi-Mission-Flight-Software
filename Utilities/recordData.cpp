#include "recordData.h"

int PRE_FLIGHT_DATA_DUMP_DURATION = 600;  //in seconds
int PRE_FLIGHT_TIME_SINCE_LAST_DUMP = 0;  //in seconds
int PRE_FLIGHT_TIME_OF_LAST_DUMP = 0;  //in seconds

// TODO this entire function has to be checked
void recordData(State& state, String stage){
    if(stage == "PreFlight"){
        dataToPSRAM(state);
        PRE_FLIGHT_TIME_SINCE_LAST_DUMP = (millis()/1000) - PRE_FLIGHT_TIME_OF_LAST_DUMP;
        if(PRE_FLIGHT_TIME_SINCE_LAST_DUMP > PRE_FLIGHT_DATA_DUMP_DURATION){
            PSRAMDumpToSD();  // TODO check the functionality of this
            resetPSRAMDumpStatus();
            PRE_FLIGHT_TIME_OF_LAST_DUMP = millis()/1000;
        }
    }
    else if(stage == "Flight"){
        psramMarkLiftoff();  // TODO this should only have to run the first time
        dataToPSRAM(state);
    }
    else if(stage == "PostFlight"){
        if(!isPSRAMDumped()){
            PSRAMDumpToSD();
        }
    }
    else{
       // TODO create error code here
    }
}

void dataToPSRAM(State& state){
    state.setdataString();
    String data = state.getdataString();
    if(isPSRAMReady()){
        psramPrintln(data);
    }
}