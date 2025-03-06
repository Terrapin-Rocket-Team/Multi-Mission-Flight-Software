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
        State(Sensor **sensors, int numSensors, Filter *filter);
        virtual ~State();

        // Returns false if any sensor failed to init. Check data log for failed sensor. Disables sensor if failed.
        // useBiasCorrection: whether or not to use bias correction. If true, the override class must call sensor.useBiasCorrection(false) upon liftoff to disable bias correction.
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
        virtual int getNumMaxSensors() const { return maxNumSensors; } // how many sensors were passed in the constructor
        virtual Sensor **getSensors() const { return sensors; }
        virtual int getStage() const { return stage; }

        bool sensorOK(const Sensor *sensor) const;



    protected:
        double currentTime; // in s since uC turned on
        double lastTime;

        int maxNumSensors; // how many sensors were passed in the constructor
        Sensor **sensors;
        int numSensors; // how many sensors are actually enabled

        // ----

        // Update functions:
        /* update(){
            updateSensors();
            updateVariables(){
                if (filter)
                    updateKF();
                else
                    updateWithoutKF();
                // other variables
            }
            determineStage();
        }
        */
        virtual void updateSensors();
        virtual void updateVariables();
        virtual void updateKF();
        virtual void updateWithoutKF(); // if no KF is set

        virtual void determineStage() = 0; // MUST BE IMPLEMENTED IN OVERRIDE CLASS

        // ----

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
        Filter *filter;

        bool initialized = false;

        int stage = 0;

        double *stateVars = nullptr;
    };
}
#endif // STATE_H