#ifndef STATE_H
#define STATE_H

#include "../Filters/Filter.h"

// Include all the sensor classes
#include "../Sensors/Baro/Barometer.h"
#include "../Sensors/GPS/GPS.h"
#include "../Sensors/IMU/IMU.h"
#include "../RecordData/Logger.h"
#include "../Constants.h"
#include "../Math/Vector.h"
#include "../Math/Quaternion.h"

namespace mmfs
{
    class State : public DataReporter
    {
    public:
        // make numSensors the number of sensors you expect to have. This is used to determine how many sensors to expect in the sensorOrder array.
        // Fill sensorOrder with one of each kind of sensor you expect to have. This is used to determine which sensor is which in the sensors[] array.
        // example if you have more than one of the same sensor type:
        // SensorType sensorOrder[numSensors] = {BAROMETER_, BAROMETER_, GPS_, IMU_}; or
        // SensorType sensorOrder[numSensors] = {BAROMETER_, GPS_, IMU_, BAROMETER_}; It doesn't what order they're in, as long as they're in the array.
        // useKalmanFilter: whether or not to use the Kalman Filter. If false, the state will use the raw sensor data.
        // stateRecordsOwnData: whether or not the state should call recordFlightData() itself. If false, other funcitons must call recordFlightData() to record the state's data.
        State(Sensor **sensors, int numSensors, Filter *filter);
        virtual ~State();

        // to be called after all applicable sensors have been added.
        // Returns false if any sensor failed to init. Check data log for failed sensor. Disables sensor if failed.
        // useBiasCorrection: whether or not to use bias correction. If true, the override class for State must implement markLiftoff() to disable bias correction.
        virtual bool init(bool useBiasCorrection = false);

        // Updates the state with the most recent sensor data. CurrentTime is the time in seconds since the uC was turned on. If not provided, the state will use the current time.
        virtual void updateState(double currentTime = -1);

        // sensor functions
        virtual Sensor *getSensor(SensorType type, int sensorNum = 1) const; // get a sensor of a certain type. 1 indexed. i.e. getSensor(GPS, 1) gets the first GPS sensor.

        // State Getters
        virtual Vector<3> getPosition() const { return position; }
        virtual Vector<3> getVelocity() const { return velocity; }
        virtual Vector<3> getAcceleration() const { return acceleration; }
        virtual Quaternion getOrientation() const { return orientation; }
        virtual Vector<2> getCoordinates() const { return coordinates; } // lat long in decimal degrees
        virtual double getHeading() const { return heading; }
        virtual int getNumMaxSensors() const { return maxNumSensors; }
        virtual Sensor **getSensors() const { return sensors; }
        virtual int getStage() const { return stage; }
        bool sensorOK(const Sensor *sensor) const;

        // State Setters
        virtual void setUseFilter(bool useFilter) { this->useFilter = useFilter; }

        // DataReporter functions
        virtual const char *getName() const override { return "State"; }

        //MUST BE IMPLEMENTED IN OVERRIDE CLASS
        virtual void determineStage() = 0;

    protected:

        double currentTime; // in s since uC turned on
        int lastTime;

        int maxNumSensors;
        Sensor **sensors;
        int numSensors; // how many sensors are actually enabled

        virtual void updateSensors();

        // Helper functions

        // State variables
        Vector<3> position;     // in m from start position
        Vector<3> velocity;     // in m/s
        Vector<3> acceleration; // in m/s^2
        Quaternion orientation; // in quaternion
        Vector<2> coordinates;  // in lat, lon
        double heading;         // in degrees

        // These two only exist because of bugs in the KF. They will be removed when the KF is fixed.
        double baroVelocity;    // in m/s
        double baroOldAltitude; // in m

        // Kalman Filter settings
        bool useFilter = true;
        Filter *filter;

        bool initialized = false;

        int stage = 0;
    };
}
#endif // STATE_H