#include "State.h"
#pragma region Constructor and Destructor

namespace mmfs
{

    State::State(Sensor **sensors, int numSensors, Filter *filter)
    {
        baroOldAltitude = 0;
        baroVelocity = 0;
        lastTime = 0;
        currentTime = 0;
        this->maxNumSensors = numSensors;
        this->sensors = sensors;
        this->filter = filter;
        useFilter = filter != nullptr;

        addColumn(DOUBLE, &currentTime, "Time (s)");
        addColumn(DOUBLE, &position.x(), "PX (m)");
        addColumn(DOUBLE, &position.y(), "PY (m)");
        addColumn(DOUBLE, &position.z(), "PZ (m)");
        addColumn(DOUBLE, &velocity.x(), "VX (m/s)");
        addColumn(DOUBLE, &velocity.y(), "VY (m/s)");
        addColumn(DOUBLE, &velocity.z(), "VZ (m/s)");
        addColumn(DOUBLE, &acceleration.x(), "AX (m/s/s)");
        addColumn(DOUBLE, &acceleration.y(), "AY (m/s/s)");
        addColumn(DOUBLE, &acceleration.z(), "AZ (m/s/s)");
    }

    State::~State()
    {
        delete filter;
    }

#pragma endregion

    bool State::init(bool useBiasCorrection)
    {
        char *logData = new char[100];
        int good = 0, tryNumSensors = 0;
        for (int i = 0; i < maxNumSensors; i++)
        {
            if (sensors[i])
            {
                tryNumSensors++;
                if (sensors[i]->begin(useBiasCorrection))
                {
                    good++;
                    snprintf(logData, 100, "%s [%s] initialized.", sensors[i]->getTypeString(), sensors[i]->getName());
                    getLogger().recordLogData(INFO_, logData);
                }
                else
                {
                    snprintf(logData, 100, "%s [%s] failed to initialize.", sensors[i]->getTypeString(), sensors[i]->getName());
                    getLogger().recordLogData(ERROR_, logData);
                }
            }
            else
            {
                snprintf(logData, 100, "A sensor in the array was null!");
                getLogger().recordLogData(ERROR_, logData);
            }
        }
        if (useFilter)
        {
            filter->initialize();
        }
        numSensors = good;

        // Set up the packed data array's size
        int size = sizeof(float) * 10; // 10 for the state data
        for (int i = 0; i < numSensors; i++)
            size += sensors[i]->getPackedDataSize();

        initialized = true;
        return good == tryNumSensors;
    }

    void State::updateSensors()
    {
        for (int i = 0; i < maxNumSensors; i++)
        {
            if (sensorOK(sensors[i]))
            { // not nullptr and initialized
                sensors[i]->update();
            }
        }
    }

    void State::updateState(double newTime)
    {
        lastTime = currentTime;
        if (newTime != -1)
            currentTime = newTime;
        else
            currentTime = millis() / 1000.0;

        updateSensors();
        GPS *gps = reinterpret_cast<GPS *>(getSensor(GPS_));
        IMU *imu = reinterpret_cast<IMU *>(getSensor(IMU_));
        Barometer *baro = reinterpret_cast<Barometer *>(getSensor(BAROMETER_));

        if (useFilter && sensorOK(imu) && sensorOK(baro)) // we only really care about Z filtering.
        {
            double *measurements = new double[filter->getMeasurementSize()];
            double *inputs = new double[filter->getInputSize()];
            double *stateVars = new double[filter->getStateSize()];

            // gps x y barometer z
            measurements[0] = sensorOK(gps) ? gps->getDisplacement().x() : 0;
            measurements[1] = sensorOK(gps) ? gps->getDisplacement().y() : 0;
            measurements[2] = baro->getAGLAltM();

            // imu x y z
            inputs[0] = acceleration.x() = imu->getAccelerationGlobal().x();
            inputs[1] = acceleration.y() = imu->getAccelerationGlobal().y();
            inputs[2] = acceleration.z() = imu->getAccelerationGlobal().z() - 9.81;

            stateVars[0] = position.x();
            stateVars[1] = position.y();
            stateVars[2] = position.z();
            stateVars[3] = velocity.x();
            stateVars[4] = velocity.y();
            stateVars[5] = velocity.z();

            filter->iterate(currentTime - lastTime, stateVars, measurements, inputs);
            // pos x, y, z, vel x, y, z
            position.x() = stateVars[0];
            position.y() = stateVars[1];
            position.z() = stateVars[2];
            velocity.x() = stateVars[3];
            velocity.y() = stateVars[4];
            velocity.z() = stateVars[5];

            if (sensorOK(baro))
            {
                baroVelocity = (baro->getAGLAltM() - baroOldAltitude) / (currentTime - lastTime);
                baroOldAltitude = baro->getAGLAltM();
            }

            delete[] stateVars;
        }
        else
        {
            if (sensorOK(gps))
            {
                position = gps->getDisplacement();
            }
            if (sensorOK(baro))
            {
                position.z() = baro->getAGLAltM();
                baroVelocity = velocity.z() = (baro->getAGLAltM() - baroOldAltitude) / (currentTime - lastTime);
                baroOldAltitude = position.z() = baro->getAGLAltM();
            }
            if (sensorOK(imu))
            {
                acceleration = imu->getAccelerationGlobal();
            }
        }
        if (sensorOK(gps))
        {
            coordinates = gps->getHasFirstFix() ? Vector<2>(gps->getPos().x(), gps->getPos().y()) : Vector<2>(0, 0);
            heading = gps->getHeading();
        }
        else
        {
            coordinates = Vector<2>(0, 0);
            heading = 0;
        }

        orientation = sensorOK(imu) ? imu->getOrientation() : Quaternion(1, 0, 0, 0);
    }

    Sensor *State::getSensor(SensorType type, int sensorNum) const
    {
        for (int i = 0; i < maxNumSensors; i++)
            if (sensors[i] && type == sensors[i]->getType() && --sensorNum == 0)
                return sensors[i];
        return nullptr;
    }

#pragma region Helper Functions

    bool State::sensorOK(const Sensor *sensor) const
    {
        if (sensor && *sensor) // not nullptr and initialized
            return true;
        return false;
    }
#pragma endregion

} // namespace mmfs