#include "State.h"
#include <Arduino.h>
#pragma region Constructor and Destructor

namespace mmfs
{
    State::State(Sensor **sensors, int numSensors, Filter *filter)
    {
        setName("State");
        baroOldAltitude = 0;
        baroVelocity = 0;
        lastTime = 0;
        currentTime = 0;
        this->maxNumSensors = numSensors;
        this->sensors = sensors;
        this->filter = filter;

        addColumn(DOUBLE, &currentTime, "Time (s)");
        addColumn(INT, &stage, "Stage");
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
        delete[] stateVars;
    }

#pragma endregion

    bool State::init(bool useBiasCorrection)
    {
        int good = 0, tryNumSensors = 0;
        for (int i = 0; i < maxNumSensors; i++)
        {
            if (sensors[i])
            {
                tryNumSensors++;
                if (sensors[i]->begin(useBiasCorrection))
                {
                    good++;
                    getLogger().recordLogData(INFO_, 100, "%s [%s] initialized.", sensors[i]->getTypeString(), sensors[i]->getName());
                }
                else
                {
                    getLogger().recordLogData(ERROR_, 100, "%s [%s] failed to initialize.", sensors[i]->getTypeString(), sensors[i]->getName());
                }
            }
            else
            {
                getLogger().recordLogData(ERROR_, "A sensor in the array was null!");
            }
        }
        if (filter){
            filter->initialize();
            stateVars = new double[filter->getStateSize()];
        }

        numSensors = good;

        initialized = true;
        if (good == tryNumSensors)
            getLogger().recordLogData(INFO_, "State Initialized. All sensors OK.");
        else
            getLogger().recordLogData(WARNING_, "State Initialized. At least one sensor failed to initialize.");
        return good == tryNumSensors;
    }

#pragma region Update Functions

    void State::updateState(double newTime)
    {
        lastTime = currentTime;
        if (newTime != -1)
            currentTime = newTime;
        else
            currentTime = millis() / 1000.0;
        updateSensors();
        updateVariables();
        determineStage();
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

    void State::updateVariables()
    {
        GPS *gps = reinterpret_cast<GPS *>(getSensor("GPS"_i));
        IMU *imu = reinterpret_cast<IMU *>(getSensor("IMU"_i));
        // Barometer *baro = reinterpret_cast<Barometer *>(getSensor(BAROMETER_));

        if (filter)
            updateKF();
        else
            updateWithoutKF();

        if (sensorOK(gps))
        {
            coordinates = gps->getHasFix() ? Vector<2>(gps->getPos().x(), gps->getPos().y()) : Vector<2>(0, 0);
            heading = gps->getHeading();
        }
        else
        {
            coordinates = Vector<2>(0, 0);
            heading = 0;
        }

        orientation = sensorOK(imu) ? imu->getOrientation() : Quaternion(1, 0, 0, 0);
    }

    void State::updateWithoutKF()
    {

        GPS *gps = reinterpret_cast<GPS *>(getSensor("GPS"_i));
        IMU *imu = reinterpret_cast<IMU *>(getSensor("IMU"_i));
        Barometer *baro = reinterpret_cast<Barometer *>(getSensor("Barometer"_i));

        if (sensorOK(gps))
            position = gps->getDisplacement();

        if (sensorOK(baro))
        {
            position.z() = baro->getAGLAltM();
            baroVelocity = velocity.z() = (baro->getAGLAltM() - baroOldAltitude) / (currentTime - lastTime);
            baroOldAltitude = position.z() = baro->getAGLAltM();
        }

        if (sensorOK(imu))
            acceleration = imu->getAccelerationGlobal();
    }

    void State::updateKF()
    {
        GPS *gps = reinterpret_cast<GPS *>(getSensor("GPS"_i));
        IMU *imu = reinterpret_cast<IMU *>(getSensor("IMU"_i));
        Barometer *baro = reinterpret_cast<Barometer *>(getSensor("Barometer"_i));

        double *measurements = new double[filter->getMeasurementSize()];
        double *inputs = new double[filter->getInputSize()];

        // gps x y barometer z
        measurements[0] = sensorOK(gps) ? gps->getDisplacement().x() : 0;
        measurements[1] = sensorOK(gps) ? gps->getDisplacement().y() : 0;
        measurements[2] = baro->getAGLAltM();

        // imu x y z
        inputs[0] = acceleration.x() = imu->getAccelerationGlobal().x();
        inputs[1] = acceleration.y() = imu->getAccelerationGlobal().y();
        inputs[2] = acceleration.z() = imu->getAccelerationGlobal().z();

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
    }

#pragma endregion Update Functions

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