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
    if(STATE.successfulSetup()){
        Serial.println("Success!");
    }
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
    Serial.print("Euler Orientation Roll: "); Serial.println(STATE.stateIMU.absoluteOrientationEuler.x());
    Serial.print("Euler Orientation Pitch: "); Serial.println(STATE.stateIMU.absoluteOrientationEuler.y());
    Serial.print("Euler Orientation Yaw: "); Serial.println(STATE.stateIMU.absoluteOrientationEuler.z());

    // Calibration for the BNO055 chip
    // uint8_t bno_cal_system, bno_cal_gyro, bno_cal_accel, bno_cal_mag = 0;
    // bno.getCalibration(&bno_cal_system, &bno_cal_gyro, &bno_cal_accel, &bno_cal_mag);

    // Serial.print("BNO Calibration Sys: "); Serial.println(bno_cal_system);
    // Serial.print("BNO Calibration Accel: "); Serial.println(bno_cal_accel);
    // Serial.print("BNO Calibration Gyro: "); Serial.println(bno_cal_gyro);
    // Serial.print("BNO Calibration Mag: "); Serial.println(bno_cal_mag);
}