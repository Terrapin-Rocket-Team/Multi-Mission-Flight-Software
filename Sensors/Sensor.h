#ifndef SENSOR_H
#define SENSOR_H

#include "../RecordData/RecordData.h"

enum SensorType
{
    BAROMETER_,
    GPS_,
    IMU_,
    LIGHT_SENSOR_,
    RADIO_,
    RTC_,
    UNKNOWN_
};
class Sensor
{
public:
    virtual ~Sensor(){};
    // Sets up the sensor and stores any critical parameters
    virtual bool initialize() = 0;
    // gives the names of the columns which transient data will be stored under, in a comma separated string
    virtual const char *getCsvHeader() const = 0;
    // gives the data values of the variables said to be stored by the header, in the same order, in a comma separated string
    virtual const char *getDataString() const = 0;
    // gives a string which includes all the sensor's static/initialization data. This will be in the format of dataName:dataValue, with each pair separated by a newline (\n)
    virtual const char *getStaticDataString() const = 0;

    virtual SensorType getType() const = 0;              // Returns the type of the sensor
    virtual const char *getTypeString() const = 0;       // Returns the type of the sensor as a string
    virtual const char *getName() const { return name; } // Returns the name of the sensor
    virtual void setName(const char *n)                  // Sets the name of the sensor
    {
        delete[] name;
        int len = strlen(n);
        name = new char[len + 1];
        snprintf(name, len + 1, "%s", n);
    }

    virtual void update() = 0; // Updates the sensor's fields by querying the sensor for new data

    virtual bool isInitialized() const { return initialized; } // Returns whether the sensor has been initialized or not

    virtual explicit operator bool() const { return initialized; } // Returns whether the sensor has been initialized or not

    virtual void setBiasCorrectionMode(bool mode) { biasCorrectionMode = mode; } // Sets the bias correction mode of the sensor
protected:
    bool initialized = false;
    bool biasCorrectionMode = true;
    char *name = nullptr;
};

#endif // SENSOR_H