#include "BNO055.h"

bool setupBNO055(){
    if(!bno.begin()){
        Serial.println("BNO055 failed to begin");
        return false;
    }
    bno.setExtCrystalUse(true);
    return true;
}

void calibrate_bno(){
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
    bno.getCalibration(&system, &gyro, &accel, &mag);
    i = i + 1;
    if (i == 10)
    { // Only want to print every 10 iterations
       Serial.print("CALIBRATION: Sys=");
       Serial.print(system, DEC);
       Serial.print(" Gyro=");
       Serial.print(gyro, DEC);
       Serial.print(" Accel=");
       Serial.print(accel, DEC);
       Serial.print(" Mag=");
       Serial.println(mag, DEC);
      i = 0;
    }
    delay(10);
  }
//   Serial.println("BNO055 Calibrated");
}

imu::Vector<3> getBNO055acceleration(){
    imu::Quaternion orientation = bno.getQuat();
    orientation.normalize();
    imu::Quaternion orientationConj = orientation.conjugate();

    imu::Vector<3> rocketFrameAcceleration = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
    imu::Quaternion rocketFrameAccelerationQuat;
    rocketFrameAccelerationQuat.w() = 0;
    rocketFrameAccelerationQuat.x() = rocketFrameAcceleration.x();
    rocketFrameAccelerationQuat.y() = rocketFrameAcceleration.y();
    rocketFrameAccelerationQuat.z() = rocketFrameAcceleration.z();

    imu::Quaternion absoluteAccelerationQuat = quatMultiplication(quatMultiplication(orientation, rocketFrameAccelerationQuat), orientationConj);
    imu::Vector<3> absoluteAcceleration;
    absoluteAcceleration.x() = absoluteAccelerationQuat.x();
    absoluteAcceleration.y() = absoluteAccelerationQuat.y();
    absoluteAcceleration.z() = absoluteAccelerationQuat.z();

    return absoluteAcceleration;
}

imu::Vector<3> getBNO055angularVelocity(){
    return bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
}

imu::Vector<3> getBNO055magneticFieldStrength(){
    return bno.getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER);
}

imu::Quaternion getBNO055absoluteOrientation(){
    imu::Quaternion orientation = bno.getQuat();
    orientation.normalize();
    return orientation;
}

imu::Quaternion quatMultiplication(imu::Quaternion quatL, imu::Quaternion quatR){
  imu::Quaternion ans;
  ans.w() = (quatL.w()*quatR.w())-(quatL.x()*quatR.x())-(quatL.y()*quatR.y())-(quatL.z()*quatR.z());
  ans.x() = (quatL.w()*quatR.x())+(quatL.x()*quatR.w())+(quatL.y()*quatR.z())-(quatL.z()*quatR.y());
  ans.y() = (quatL.w()*quatR.y())-(quatL.x()*quatR.z())+(quatL.y()*quatR.w())+(quatL.z()*quatR.x());
  ans.z() = (quatL.w()*quatR.z())+(quatL.x()*quatR.y())-(quatL.y()*quatR.x())+(quatL.z()*quatR.w());
  return ans;
}