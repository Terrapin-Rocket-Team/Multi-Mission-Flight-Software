#include <Arduino.h>
#include "ExampleState.h"
#include <Multi_Mission_Flight_Software.h>

Barometer frameBarometer("BMP280");
ExampleState EXAMPLESTATE;

void setup() {
    //add sensors
    EXAMPLESTATE.addBarometer(frameBarometer);

    //setup steps
    setupPSRAM(EXAMPLESTATE.csvHeader);
    setupSDCard(EXAMPLESTATE.csvHeader);
    EXAMPLESTATE.stateBarometer.setupBarometer();
}

void loop() {
    EXAMPLESTATE.settimeAbsolute();
    EXAMPLESTATE.stateBarometer.readBarometer();

    EXAMPLESTATE.determineFrameStage();

    recordData(EXAMPLESTATE, EXAMPLESTATE.getrecordDataState());
}
