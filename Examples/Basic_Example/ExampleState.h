#ifndef EXAMPLE_STATE_H
#define EXAMPLE_STATE_H

#include <Multi_Mission_Flight_Software.h>

class ExampleState: public State{
public:
    void determineExampleStage();

private:

};

void ExampleState::determineExampleStage(){
    //Stages: <List Stages here> ex. '"Pre Launch", "Powered Ascent", etc.'
    determinetimeSincePreviousStage();
    if(stage == "Pre Launch" && timeSincePreviousStage > 5){ // Change stage to powered ascent after 5 seconds
        timeLaunch = millis();
        timePreviousStage = millis();
        stage = "Powered Ascent";
        recordDataStage = "Flight";
    }
    else if(stage == "Powered Ascent" && timeSincePreviousStage > 2){
        timePreviousStage = millis();
        stage = "Coasting";
    }
    // continue else if's for every stage
}

#endif
