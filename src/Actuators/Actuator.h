#ifndef ACTUATOR_H
#define ACTUATOR_H

#include "../RecordData/RecordData.h"

namespace mmfs
{

enum ActuatorType
{
    SERVO_,
    UNKNOWN_
};
class Actuator
{
public:
    virtual ~Actuator(){
        delete[] name;
    };
    // Sets up the actuator and stores any critical parameters
    virtual bool initialize() = 0;
    // gives the names of the columns which transient data will be stored under, in a comma separated string
    virtual const char *getCsvHeader() const = 0;
    // gives the data values of the variables said to be stored by the header, in the same order, in a comma separated string
    virtual const char *getDataString() const = 0;

    virtual ActuatorType getType() const = 0;             // Returns the type of the actuator
    virtual const char *getTypeString() const = 0;       // Returns the type of the actuator as a string
    virtual const char *getName() const { return name; } // Returns the name of the actuator
    virtual void setName(const char *n)                  // Sets the name of the actuator
    {
        delete[] name;
        int len = strlen(n);
        name = new char[len + 1];
        snprintf(name, len + 1, "%s", n);
    }

    virtual bool isInitialized() const { return initialized; } // Returns whether the actuator has been initialized or not

    virtual explicit operator bool() const { return initialized; } // Returns whether the actuator has been initialized or not

protected:
    bool initialized = false;
    char *name = nullptr;
};

}

#endif // ACTUATOR_H