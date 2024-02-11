#include <Arduino.h>
#include <Multi_Mission_Flight_Software.h>

int BUZZER_PIN = 9;

State STATE;

void setup() {
    buzz(BUZZER_PIN, 1000, 1);
    Serial.begin(115200);

    Serial.print("Setting up PSRAM and SD Card...");
    STATE.setcsvHeader();
    setupPSRAM(STATE.csvHeader);
    bool sdSuccess = setupSDCard(STATE.csvHeader);
    if(sdSuccess){
        Serial.println("Success!");
    }
    else{
        Serial.println("Failed! Try rebooting.");
    }
    

    Serial.print("Testing Buzzer...");
    buzz(BUZZER_PIN, 3000, 1);
    Serial.println("Success!");
}

void loop() {
  STATE.settimeAbsolute();

  Serial.println("Testing Record Data");

  STATE.setdataString();
  recordData(STATE.getdataString(), "PreFlight");

  Serial.println("Basic Teensy Test was a Success!");
  while(1);
}