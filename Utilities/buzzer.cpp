#include "buzzer.h"

void buzz(int buzzer_pin, int durationOfBuzz, int counter){
    pinMode(buzzer_pin, OUTPUT);
    for (int i = 0; i < counter; i++) {
        digitalWrite(buzzer_pin, HIGH);
        delay(durationOfBuzz);
        digitalWrite(buzzer_pin, LOW);
        delay(1000);
    }
}