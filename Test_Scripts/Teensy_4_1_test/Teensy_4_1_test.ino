#include <Arduino.h>
#include <Multi_Mission_Flight_Software.h>

int BUZZER_PIN = 9;

State STATE;

void setup() {
    buzz(BUZZER_PIN, 1000);
    Serial.begin(115200);

    Serial.print("Setting up PSRAM and SD Card...");
    STATE.setcsvHeader();
    setupPSRAM(STATE.csvHeader);
    setupSDCard(STATE.csvHeader);
    Serial.println("Success!");

    Serial.print("Testing Buzzer...");
    buzz(BUZZER_PIN, 3000);
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