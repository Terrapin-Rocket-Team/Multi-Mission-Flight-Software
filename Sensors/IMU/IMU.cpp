#include "IMU.h"

IMU::IMU(){
    sensorName = "";
    setupSuccessful = false;
    accelerationFlag = false; angularVelocityFlag = false;
    magneticFieldStrengthFlag = false; absoluteOrientationFlag = false;
    absoluteOrientationEulerFlag = false;
}

IMU::IMU(std::string inputSensorName){
    sensorName = inputSensorName;
    setupSuccessful = false;
    accelerationFlag = false; angularVelocityFlag = false;
    magneticFieldStrengthFlag = false; absoluteOrientationFlag = false;
    absoluteOrientationEulerFlag = false;
}

void IMU::setupIMU(){
    if(sensorName == "BNO055"){
        setupSuccessful = setupBNO055();
        accelerationFlag = true; angularVelocityFlag = true;
        magneticFieldStrengthFlag = true; absoluteOrientationFlag = true;
        absoluteOrientationEulerFlag = true;
    }
    if(sensorName == "High Accel BNO055"){
        setupSuccessful = setupHighAccelBNO055();
        accelerationFlag = true; angularVelocityFlag = true;  // TODO check if they get angularvelo and mag too, might just be accel
        magneticFieldStrengthFlag = true;
    }
}

void IMU::readIMU(){
    if(!setupSuccessful){
        return;
    }
    if(sensorName == "BNO055"){
        acceleration = getBNO055acceleration();
        angularVelocity = getBNO055angularVelocity();
        magneticFieldStrength = getBNO055magneticFieldStrength();
        absoluteOrientation = getBNO055absoluteOrientation();
        quatToXYZ();
    }
    if(sensorName == "High Accel BNO055"){
        acceleration = getHighAccelBNO055acceleration();
        angularVelocity = getHighAccelBNO055angularVelocity();
        magneticFieldStrength = getHighAccelBNO055magneticFieldStrength();
    }
}

void IMU::setcsvHeader(){
    csvHeader = "";
    if(accelerationFlag){
        csvHeader += "IMU "; csvHeader += "AccelX"; csvHeader += ",";
        csvHeader += "IMU "; csvHeader += "AccelY"; csvHeader += ",";
        csvHeader += "IMU "; csvHeader += "AccelZ"; csvHeader += ",";
    }
    if(angularVelocityFlag){
        csvHeader += "IMU "; csvHeader += "AngVeloX"; csvHeader += ",";
        csvHeader += "IMU "; csvHeader += "AngVeloY"; csvHeader += ",";
        csvHeader += "IMU "; csvHeader += "AngVeloZ"; csvHeader += ",";
    }
    if(magneticFieldStrengthFlag){
        csvHeader += "IMU "; csvHeader += "MagX"; csvHeader += ",";
        csvHeader += "IMU "; csvHeader += "MagY"; csvHeader += ",";
        csvHeader += "IMU "; csvHeader += "MagZ"; csvHeader += ",";
    }
    if(absoluteOrientationFlag){
        csvHeader += "IMU "; csvHeader += "OrientationW"; csvHeader += ",";
        csvHeader += "IMU "; csvHeader += "OrientationX"; csvHeader += ",";
        csvHeader += "IMU "; csvHeader += "OrientationY"; csvHeader += ",";
        csvHeader += "IMU "; csvHeader += "OrientationZ"; csvHeader += ",";
    }
    if(absoluteOrientationEulerFlag){
        csvHeader += "IMU "; csvHeader += "Roll"; csvHeader += ",";
        csvHeader += "IMU "; csvHeader += "Pitch"; csvHeader += ",";
        csvHeader += "IMU "; csvHeader += "Yaw"; csvHeader += ",";
    }
}

String IMU::getcsvHeader(){
    return csvHeader;
}

void IMU::setdataString(){
    dataString = "";
    if(accelerationFlag){
        dataString += String(acceleration.x()); dataString += ",";
        dataString += String(acceleration.y()); dataString += ",";
        dataString += String(acceleration.z()); dataString += ",";
    }
    if(angularVelocityFlag){
        dataString += String(angularVelocity.x()); dataString += ",";
        dataString += String(angularVelocity.y()); dataString += ",";
        dataString += String(angularVelocity.z()); dataString += ",";
    }
    if(magneticFieldStrengthFlag){
        dataString += String(magneticFieldStrength.x()); dataString += ",";
        dataString += String(magneticFieldStrength.y()); dataString += ",";
        dataString += String(magneticFieldStrength.z()); dataString += ",";
    }
    if(absoluteOrientationFlag){
        dataString += String(absoluteOrientation.w()); dataString += ",";
        dataString += String(absoluteOrientation.x()); dataString += ",";
        dataString += String(absoluteOrientation.y()); dataString += ",";
        dataString += String(absoluteOrientation.z()); dataString += ",";
    }
    if(absoluteOrientationEulerFlag){
        dataString += String(absoluteOrientationEuler.x()); dataString += ",";
        dataString += String(absoluteOrientationEuler.y()); dataString += ",";
        dataString += String(absoluteOrientationEuler.z()); dataString += ",";
    }
}

String IMU::getdataString(){
    return dataString;
}

void IMU::quatToXYZ(){
    imu::Vector<3> XYZ;
    imu::Quaternion q = absoluteOrientation;
    double qw = q.w(), qx = q.x(), qy = q.y(), qz = q.z();

    // 3-2-1 Euler Angles
    XYZ.x() = atan2(2 * (qw*qx + qy*qz), 1 - 2 * (qx*qx + qy*qy));//-pi to pi
    XYZ.y() = asin(2 * (qw*qy - qz*qx));//-pi/2 to pi/2
    XYZ.z() = atan2(2 * (qw*qz + qx*qy), 1 - 2 * (qy*qy + qz*qz));//-pi to pi
    // 3-1-3 Euler Angles
    // XYZ.x() = atan2(2 * (qx*qy + qw*qz), qw*qw - qx*qx - qy*qy + qz*qz);
    // XYZ.y() = asin(2 * (qw*qy - qx*qz));
    // XYZ.z() = atan2(2 * (qx*qy + qw*qz), qw*qw + qx*qx - qy*qy - qz*qz);
   
    XYZ.toDegrees(); //function from vector.h

    absoluteOrientationEuler = XYZ;
}
