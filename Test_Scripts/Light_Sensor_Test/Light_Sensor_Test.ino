#include <Arduino.h>
#include <Multi_Mission_Flight_Software.h>

State STATE;
LightSensor TESTLIGHTSENSOR("BH1750"); //Put Light Sensor type here

void setup() {
    Serial.begin(115200);

    Serial.println("Adding Light Sensor");
    STATE.addLightSensor(TESTLIGHTSENSOR);

    Serial.print("Setting up Light Sensor...");
    STATE.stateLightSensor.setupLightSensor();
    if(STATE.successfulSetup()){
        Serial.println("Success!");
    }
}

void loop() {

    STATE.stateLightSensor.readLightSensor();
    
    Serial.print("Lux: "); Serial.println(STATE.stateLightSensor.lux);
}
