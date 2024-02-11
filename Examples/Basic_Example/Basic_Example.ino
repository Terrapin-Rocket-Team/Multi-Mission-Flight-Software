#include <Arduino.h>
#include "ExampleState.h"
#include <Multi_Mission_Flight_Software.h>

Barometer frameBarometer("BMP280");
ExampleState EXAMPLESTATE;

int BUZZER_PIN = 1;

void setup() {
    buzz(BUZZER_PIN, 1000, 1);

    //add sensors
    EXAMPLESTATE.addBarometer(frameBarometer);

    //setup steps
    setupPSRAM(EXAMPLESTATE.csvHeader);
    bool sdSuccess = setupSDCard(EXAMPLESTATE.csvHeader);
    EXAMPLESTATE.stateBarometer.setupBarometer();

    if(sdSuccess){
        buzz(BUZZER_PIN, 3000, 1);
    }
    else{
        buzz(BUZZER_PIN, 250, 5);
    }
}

void loop() {
    EXAMPLESTATE.settimeAbsolute();
    EXAMPLESTATE.stateBarometer.readBarometer();

    EXAMPLESTATE.determineExampleStage();

    EXAMPLESTATE.setdataString();
    recordData(EXAMPLESTATE.getdataString(), EXAMPLESTATE.getrecordDataStage());
}
