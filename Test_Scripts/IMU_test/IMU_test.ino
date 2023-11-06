#include <Arduino.h>
#include <Multi_Mission_Flight_Software.h>

State STATE;
IMU TESTIMU("BNO055"); //Input IMU type here

void setup() {
    Serial.begin(115200);


    Serial.println("Adding IMU");
    STATE.addIMU(TESTIMU);

    Serial.print("Setting up IMU...");
    STATE.stateIMU.setupIMU();
    Serial.println("Success!");
}

void loop() {
    STATE.settimeAbsolute();

    STATE.stateIMU.readIMU();
    Serial.print("Accel X: "); Serial.println(STATE.stateIMU.acceleration.x());
    Serial.print("Accel Y: "); Serial.println(STATE.stateIMU.acceleration.y());
    Serial.print("Accel Z: "); Serial.println(STATE.stateIMU.acceleration.z());
    Serial.print("Angualar Velo X: "); Serial.println(STATE.stateIMU.angularVelocity.x());
    Serial.print("Angualar Velo Y: "); Serial.println(STATE.stateIMU.angularVelocity.y());
    Serial.print("Angualar Velo Z: "); Serial.println(STATE.stateIMU.angularVelocity.z());
    Serial.print("Magnetic X: "); Serial.println(STATE.stateIMU.magneticFieldStrength.x());
    Serial.print("Magnetic Y: "); Serial.println(STATE.stateIMU.magneticFieldStrength.y());
    Serial.print("Magnetic Z: "); Serial.println(STATE.stateIMU.magneticFieldStrength.z());
    Serial.print("Quat Orientation W: "); Serial.println(STATE.stateIMU.absoluteOrientation.w());
    Serial.print("Quat Orientation X: "); Serial.println(STATE.stateIMU.absoluteOrientation.x());
    Serial.print("Quat Orientation Y: "); Serial.println(STATE.stateIMU.absoluteOrientation.y());
    Serial.print("Quat Orientation Z: "); Serial.println(STATE.stateIMU.absoluteOrientation.z());
}