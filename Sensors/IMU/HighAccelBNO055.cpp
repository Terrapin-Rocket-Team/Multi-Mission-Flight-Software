#include "HighAccelBNO055.h"

bool setupHighAccelBNO055(){
    if(!highAccelbno.begin()){
        Serial.println("BNO055 failed to begin");
        return false;
    }
    // TODO set the bno to no fusion mode and to +/- 16gs
    return true;
}

void calibrateHighAccelBNO(){
  // Instructions on how to calibrate BNO055
  // Hold still to calibrate gyro
  // Move in figure 8 to calibrate mag
  // Steps to calibrate accel
  // 1. lay flat for 3 seconds
  // 2. tilt at 45 degree angle for 3 seconds
  // 3. tilt opposite 45 degree angle for 3 seconds
  // 4. tilt 45 degrees in other plane for 3 seconds
  // 5. tilt oppsite 45 degrees in other plane for 3 seconds
  // 6. turn upside for 3 seconds
  // 7. repeat 2-5 but start with accel upsidedown
  // 8. repeat 1-7 until fully calibrated
  uint8_t system, gyro, accel, mag, i = 0;
//   Serial.println("Calibrating BNO055");
  while ((system != 3) || (gyro != 3) || (accel != 3) || (mag != 3))
  {
    highAccelbno.getCalibration(&system, &gyro, &accel, &mag);
    i = i + 1;
    if (i == 10)
    { // Only want to print every 10 iterations
    //   Serial.print("CALIBRATION: Sys=");
    //   Serial.print(system, DEC);
    //   Serial.print(" Gyro=");
    //   Serial.print(gyro, DEC);
    //   Serial.print(" Accel=");
    //   Serial.print(accel, DEC);
    //   Serial.print(" Mag=");
    //   Serial.println(mag, DEC);
      i = 0;
    }
    delay(10);
  }
//   Serial.println("BNO055 Calibrated");
}

imu::Vector<3> getHighAccelBNO055acceleration(){
    return highAccelbno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
}

imu::Vector<3> getHighAccelBNO055angularVelocity(){
    return highAccelbno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
}

imu::Vector<3> getHighAccelBNO055magneticFieldStrength(){
    return highAccelbno.getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER);
}
