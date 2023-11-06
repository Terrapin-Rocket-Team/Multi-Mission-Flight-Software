#include <Arduino.h>
#include <Multi_Mission_Flight_Software.h>

State STATE;
Barometer TESTBAROMETER("BMP280"); //Inupt type of barometer here

void setup() {
    Serial.begin(115200);


    Serial.println("Adding Barometer");
    STATE.addBarometer(TESTBAROMETER);

    Serial.print("Setting up Barometer...");
    STATE.stateBarometer.setupBarometer();
    Serial.println("Success!");
}

void loop() {
    STATE.settimeAbsolute();

    STATE.stateBarometer.readBarometer();
    Serial.print("Pressure: "); Serial.println(STATE.stateBarometer.pressure);
    Serial.print("Temperature: "); Serial.println(STATE.stateBarometer.temperature);
    Serial.print("Altitude: "); Serial.println(STATE.stateBarometer.relativeAltitude);
}