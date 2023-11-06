#ifndef STATE_H
#define STATE_H

//Include all the sensor classes
/*#include "../Sensors/Barometer/Barometer.h"
#include "../Sensors/GPS/GPS.h"
#include "../Sensors/IMU/IMU.h"
#include "../Sensors/LightSensor/LightSensor.h"*/
#include "../Sensors/sensors.h"

class State{
public:
    double timeAbsolute;  //in s
    imu::Vector<3> position;  //in m from start position
    imu::Vector<3> velocity;  //in m/s
    imu::Vector<3> acceleration;  //in m/s^2
    String stage;
    Barometer stateBarometer;
    GPS stateGPS;
    IMU stateIMU;
    LightSensor stateLightSensor;
    String csvHeader;
    

    State();  //constructor
    void settimeAbsolute();
    void setcsvHeader();
    void setdataString();
    String getdataString();
    String getrecordDataStage();

    //add sensor functions
    void addBarometer(const Barometer& Barometer);
    void addGPS(const GPS& gps);
    void addIMU(const IMU& imu);
    void addLightSensor(const LightSensor& LightSensor);


protected:  //able to be accesses by the child classes
    double apogee;  //in m above start position
    double accelerationMagnitude;  //in m/s^2
    double timeLaunch; //in s
    double timeSinceLaunch;  //in s
    double timePreviousStage;  //in s
    double timeSincePreviousStage;  //in s

    bool barometerFlag;
    bool gpsFlag;
    bool imuFlag;
    bool lightSensorFlag;

    String recordDataStage;

    void determinetimeSincePreviousStage();
    void determinetimeSinceLaunch();
    void determineaccelerationMagnitude(imu::Vector<3> accel);
    void determineapogee(double zPosition);

private:
    String dataString;

};

#endif