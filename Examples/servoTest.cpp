//Simple Servo Test code
#include <Servo.h>

Servo testServo;
int pos = 0;

void setup(){
    testServo.attach(1); //insert pin which you want to test
}

void loop(){
    for(pos = 0; pos<=180; pos++){
        testServo.write(pos);
        delay(15);
    }
    for(pos = 180; pos>=0; pos--){
        testServo.write(pos);
        delay(15);
    }
}