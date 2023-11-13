#include "buzzer.h"

void buzz(int buzzer_pin, int durationOfBuzz){
    pinMode(buzzer_pin, OUTPUT);
    digitalWrite(buzzer_pin, HIGH);
    delay(durationOfBuzz);
    digitalWrite(buzzer_pin, LOW);
}