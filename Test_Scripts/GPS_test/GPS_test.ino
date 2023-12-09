#include <Arduino.h>
#include <Multi_Mission_Flight_Software.h>

State STATE;
GPS TESTGPS("NEO-M9N"); //Put GPS type here

int timeSinceLastPrint = millis();

void setup() {
    Serial.begin(115200);

    Serial.println("Adding GPS");
    STATE.addGPS(TESTGPS);

    Serial.print("Setting up GPS...");
    STATE.stateGPS.setupGPS();
    Serial.println("Success!");
}

void loop() {
    STATE.settimeAbsolute();

    STATE.stateGPS.readGPS();

    if(millis() - timeSinceLastPrint > 1000){
        timeSinceLastPrint = millis();
        Serial.print("Longitude: "); Serial.println(STATE.stateGPS.longitude);
        Serial.print("Latitude: "); Serial.println(STATE.stateGPS.latitude);
        Serial.print("Altitude: "); Serial.println(STATE.stateGPS.altitude);
        Serial.print("Heading Angle (from North): "); Serial.println(STATE.stateGPS.headingAngle);
        Serial.print("Satellites: "); Serial.println(STATE.stateGPS.satellites);
        Serial.print("Position X: "); Serial.println(STATE.stateGPS.gpsPosition.x());
        Serial.print("Position Y: "); Serial.println(STATE.stateGPS.gpsPosition.y());
        Serial.print("Position Z: "); Serial.println(STATE.stateGPS.gpsPosition.z());
    }
}