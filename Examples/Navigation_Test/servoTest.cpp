/*
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
*/
#include <PWMServo.h>

PWMServo myservo1;  // create servo object to control a servo
PWMServo myservo2;

void setup() {
  myservo1.attach(3);  // 1 is left
  myservo2.attach(4);    // 2 is right
}

void loop() {
  myservo1.write(100);
  delay(1500); 
  myservo2.write(80);
    delay(1500); 
  myservo1.write(70);
    delay(1500);
  myservo2.write(110);
    delay(1500); 
                 
}
