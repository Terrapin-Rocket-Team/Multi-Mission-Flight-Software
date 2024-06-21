#ifndef STATE_H
#define STATE_H

#include <Wire.h>

#include "KalmanInterface.h"

// Include all the sensor classes
#include "../Sensors/Baro/Barometer.h"
#include "../Sensors/GPS/GPS.h"
#include "../Sensors/IMU/IMU.h"
#include "../Radio/Radio.h"
#include "../RecordData/RecordData.h"
class State
{
public:
    // make numSensors the number of sensors you expect to have. This is used to determine how many sensors to expect in the sensorOrder array.
    // Fill sensorOrder with one of each kind of sensor you expect to have. This is used to determine which sensor is which in the sensors[] array.
    // example if you have more than one of the same sensor type:
    // SensorType sensorOrder[numSensors] = {BAROMETER_, BAROMETER_, GPS_, IMU_}; or
    // SensorType sensorOrder[numSensors] = {BAROMETER_, GPS_, IMU_, BAROMETER_}; It doesn't what order they're in, as long as they're in the array.
    // useKalmanFilter: whether or not to use the Kalman Filter. If false, the state will use the raw sensor data.
    // stateRecordsOwnData: whether or not the state should call recordFlightData() itself. If false, other funcitons must call recordFlightData() to record the state's data.
    State( Sensor **sensors, int numSensors, KalmanInterface *kfilter, bool stateRecordsOwnData = true);
    virtual ~State();

    // to be called after all applicable sensors have been added.
    // Returns false if any sensor failed to init. Check data log for failed sensor. Disables sensor if failed.
    virtual bool init();

    // Updates the state with the most recent sensor data. CurrentTime is the time in seconds since the uC was turned on. If not provided, the state will use the current time.
    virtual void updateState(double currentTime = -1);

    // sensor functions
    virtual Sensor *getSensor(SensorType type, int sensorNum = 1) const; // get a sensor of a certain type. 1 indexed. i.e. getSensor(GPS, 1) gets the first GPS sensor.

    virtual char *getDataString() const;
    virtual char *getCsvHeader() const;
    virtual char *getStateString(); // This contains only the portions that define what the state thinks the rocket looks like, not full sensor data.

    // State Getters
    virtual imu::Vector<3> getPosition() const { return position; }
    virtual imu::Vector<3> getVelocity() const { return velocity; }
    virtual imu::Vector<3> getAcceleration() const { return acceleration; }
    virtual imu::Quaternion getOrientation() const { return orientation; }
    virtual imu::Vector<2> getCoordinates() const { return coordinates; } // lat long in decimal degrees
    virtual double getHeading() const { return heading; }

    // State Setters
    virtual void setUseKF(bool useKF) { this->useKF = useKF; }
    virtual void setRecordOwnFlightData(bool recordOwnFlightData) { this->recordOwnFlightData = recordOwnFlightData; }


protected:
    double currentTime; // in s since uC turned on
    int lastTime;


    int maxNumSensors;
    Sensor **sensors;
    int numSensors; // how many sensors are actually enabled

    char *csvHeader;
    char *dataString;
    char *stateString;

    virtual void setCsvHeader();
    virtual void setDataString();

    virtual void updateSensors();

    // internal state variables
    bool recordOwnFlightData; // used to determine if the state should record its own data or leave it to other functions

    // Helper functions
    void setCsvString(char *str, const char *start, int startSize, bool header);
    bool sensorOK(const Sensor *sensor) const;

    // State variables
    imu::Vector<3> position;      // in m from start position
    imu::Vector<3> velocity;      // in m/s
    imu::Vector<3> acceleration;  // in m/s^2
    imu::Quaternion orientation;  // in quaternion
    imu::Vector<2> coordinates;   // in lat, lon
    double heading;               // in degrees

    // These two only exist because of bugs in the KF. They will be removed when the KF is fixed.
    double baroVelocity;    // in m/s
    double baroOldAltitude; // in m

    // Kalman Filter settings
    bool useKF;
    KalmanInterface *kfilter;
};

#endif

/*
    To add sensors: Include a local variable for the sensor in the state class and edit the applySensorType() function to point the local variable to the correct index in the sensors[] array.
    Then, edit what the state class does with the sensor data in the updateState() function.
    Finally, make sure too update the NUM_MAX_SENSORS, SENSOR_ORDER, and SENSOR_NAMES arrays to reflect the new sensor.

    It's that easy!
    Don't forget to call addSensor() with your new sensor.

    Use sensorOK() to check if a sensor is enabled before using it.

*/