#include "State.h"
#pragma region Constructor and Destructor

State::State(int maxNumSensors, SensorType *sensorOrder, KalmanInterface *kfilter, bool stateRecordsOwnFlightData)
{
    baroOldAltitude = 0;
    baroVelocity = 0;

    stateString = nullptr;
    dataString = nullptr;
    csvHeader = nullptr;
    numSensors = 0;
    this->maxNumSensors = maxNumSensors;
    sensors = new Sensor *[maxNumSensors];
    this->sensorOrder = new SensorType[maxNumSensors];
    for (int i = 0; i < maxNumSensors; i++)
    {
        sensors[i] = nullptr;
        this->sensorOrder[i] = sensorOrder[i];
    }

    recordOwnFlightData = stateRecordsOwnFlightData;
    this->kfilter = kfilter;
}

State::~State()
{
    delete[] csvHeader;
    delete[] stateString;
    delete kfilter;
    for (int i = 0; i < maxNumSensors; i++)
    {
        delete sensors[i];
    }
    delete[] sensors;
    delete[] sensorOrder;
}

#pragma endregion

bool State::init()
{
    char *logData = new char[100];
    int good = 0, tryNumSensors = 0;
    for (int i = 0; i < maxNumSensors; i++)
    {
        if (sensors[i])
        {
            tryNumSensors++;
            if (sensors[i]->initialize())
            {
                good++;
                snprintf(logData, 100, "%s [%s] initialized.", sensors[i]->getTypeString(), sensors[i]->getName());
                recordLogData(INFO, logData);
            }
            else
            {
                snprintf(logData, 100, "%s [%s] failed to initialize.", sensors[i]->getTypeString(), sensors[i]->getName());
                recordLogData(ERROR, logData);
            }
        }
        else
        {
            snprintf(logData, 100, "A sensor was not added via addSensor!");
            recordLogData(ERROR, logData);
        }
    }
    if (useKF)
    {
        kfilter = new KalmanInterface(3, 3, 6);
        kfilter->initialize();
    }
    numSensors = good;
    setCsvHeader();

    return good == tryNumSensors;
}

void State::updateSensors()
{
    for (int i = 0; i < maxNumSensors; i++)
    {
        if (sensorOK(sensors[i]))
        { // not nullptr and initialized
            sensors[i]->update();
            Wire.beginTransmission(0x42); // random address for testing the i2c bus
            byte b = Wire.endTransmission();
            if (b != 0x00)
            {
                Wire.end();
                Wire.begin();
                recordLogData(ERROR, "I2C Error");
                sensors[i]->update();
                delay(10);
                sensors[i]->update();
            }
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

    if (useKF && sensorOK(imu) && sensorOK(baro)) // we only really care about Z filtering.
    {
        double *measurements = new double[kfilter->getMeasurementSize()];
        double *inputs = new double[kfilter->getInputSize()];

        // gps x y barometer z
        measurements[0] = sensorOK(gps) ? gps->getDisplacement().x() : 0;
        measurements[1] = sensorOK(gps) ? gps->getDisplacement().y() : 0;
        measurements[2] = baro->getRelAltM();

        // imu x y z
        inputs[0] = acceleration.x() = imu->getAcceleration().x();
        inputs[1] = acceleration.y() = imu->getAcceleration().y();
        inputs[2] = acceleration.z() = imu->getAcceleration().z();

        double *predictions = kfilter->iterate(currentTime - lastTime, inputs, measurements);
        // pos x, y, z, vel x, y, z
        position.x() = predictions[0];
        position.y() = predictions[1];
        position.z() = predictions[2];
        velocity.x() = predictions[3];
        velocity.y() = predictions[4];
        velocity.z() = predictions[5];

        if (sensorOK(baro))
        {
            baroVelocity = (baro->getRelAltM() - baroOldAltitude) / (currentTime - lastTime);
            baroOldAltitude = baro->getRelAltM();
        }

        delete[] predictions;
    }
    else
    {
        if (sensorOK(gps))
        {
            position = imu::Vector<3>(gps->getDisplacement().x(), gps->getDisplacement().y(), gps->getAlt());
            velocity = gps->getVelocity();
        }
        if (sensorOK(baro))
        {
            baroVelocity = velocity.z() = (baro->getRelAltM() - baroOldAltitude) / (currentTime - lastTime);
            baroOldAltitude = position.z() = baro->getRelAltM();
        }
        if (sensorOK(imu))
        {
            acceleration = imu->getAcceleration();
        }
    }
    if (sensorOK(gps))
    {
        coordinates = gps->getHasFirstFix() ? imu::Vector<2>(gps->getPos().x(), gps->getPos().y()) : imu::Vector<2>(0, 0);
        heading = gps->getHeading();
    }
    else
    {
        coordinates = imu::Vector<2>(0, 0);
        heading = 0;
    }

    orientation = sensorOK(imu) ? imu->getOrientation() : imu::Quaternion(0, 0, 0, 1);

    setDataString();
    if (recordOwnFlightData)
        recordFlightData(dataString);
}

void State::setCsvHeader()
{
    char csvHeaderStart[] = "Time,Stage,PX,PY,PZ,VX,VY,VZ,AX,AY,AZ,";
    setCsvString(csvHeader, csvHeaderStart, sizeof(csvHeaderStart), true);
}

void State::setDataString()
{
    // Assuming 12 char/float (2 dec precision, leaving min value of -9,999,999.99), 10 char/int, 30 char/string
    // float * 9, int * 0, string * 0, 11 commas and a null terminator
    // 108 + 12 = 120
    const int dataStartSize = 12 * 9 + 12;
    char csvDataStart[dataStartSize];
    snprintf(
        csvDataStart, dataStartSize,
        "%.3f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,", // trailing comma very important
        currentTime,
        position.x(), position.y(), position.z(),
        velocity.x(), velocity.y(), velocity.z(),
        acceleration.x(), acceleration.y(), acceleration.z());
    setCsvString(dataString, csvDataStart, dataStartSize, false);
}

char *State::getStateString()
{
    delete[] stateString;
    stateString = new char[500]; // way oversized for right now.
    GPS *gps = reinterpret_cast<GPS *>(getSensor(GPS_));
    snprintf(stateString, 500, "%.3f|%.2f,%.2f,%.2f|%.2f,%.2f,%.2f|%.7f,%.7f,%.2f|%.2f,%.2f,%.2f,%.2f|%.2f",
             currentTime,
             acceleration.x(), acceleration.y(), acceleration.z(),
             velocity.x(), velocity.y(), velocity.z(),
             position.x(), position.y(), position.z(),
             orientation.x(), orientation.y(), orientation.z(), orientation.w(),
             sensorOK(gps) ? gps->getHeading() : 0);
    return stateString;
}

char *State::getDataString() const { return dataString; }

char *State::getCsvHeader() const { return csvHeader; }

#pragma region Getters and Setters for Sensors

bool State::addSensor(Sensor *sensor, int sensorNum)
{
    int modifiedNum = sensorNum;
    for (int i = 0; i < maxNumSensors; i++)
    {
        if (sensorOrder[i] == sensor->getType() && --modifiedNum == 0)
        {
            sensors[i] = sensor;
            return true;
        }
    }
    return false;
}

Sensor *State::getSensor(SensorType type, int sensorNum) const
{
    for (int i = 0; i < maxNumSensors; i++)
        if (sensorOrder[i] == sensors[i]->getType() && --sensorNum == 0)
            return sensors[i];
    return nullptr;
}

#pragma endregion

#pragma region Helper Functions

void State::setCsvString(char *dest, const char *start, int startSize, bool header)
{
    int numCategories = numSensors + 1;
    const char **str = new const char *[numCategories];
    str[0] = start;
    int cursor = 1;
    delete[] dest;
    //---Determine required size for string
    int size = startSize + 1; // includes '\0' at end of string for the end of dataString to use
    for (int i = 0; i < maxNumSensors; i++)
    {
        if (sensorOK(sensors[i]))
        {
            str[cursor] = header ? sensors[i]->getCsvHeader() : sensors[i]->getDataString();
            size += strlen(str[cursor++]);
        }
    }
    dest = new char[size];
    if (header)
        csvHeader = dest;
    else
        dataString = dest;
    //---Fill data String
    int j = 0;
    for (int i = 0; i < numCategories; i++)
    {
        for (int k = 0; str[i][k] != '\0'; j++, k++)
        { // append all the data strings onto the main string

            dest[j] = str[i][k];
        }
        if (i >= 1 && !header)
        {
            delete[] str[i]; // delete all the heap arrays.
        }
    }
    delete[] str;
    dest[j - 1] = '\0'; // all strings have ',' at end so this gets rid of that and terminates it a character early.
}

bool State::sensorOK(const Sensor *sensor) const
{
    if (sensor && *sensor) // not nullptr and initialized
        return true;
    return false;
}

#pragma endregion