#ifndef SENSOR_H
#define SENSOR_H

#include "../Constants.h"
#include "../Utils/CircBuffer.h"
#include "../BlinkBuzz/BlinkBuzz.h"
#include "../RecordData/Logging/Logger.h"
#include <algorithm>
#include "../RecordData/DataReporter/DataReporter.h"
#include "Utils/Hash.h"

namespace mmfs
{
    using SensorType = uint32_t;

    class Sensor : public DataReporter
    {
    public:
        virtual ~Sensor() {};
        // ------------------------------- SENSOR TYPE IMPLEMENTATION ---------------------------------------------

        // Child classes can override these classes with specifics for that type of sensor.
        // For instance, a barometer might read pressure but use the rest of update() to convert that to altitude.

        // Updates the sensor's fields by querying the sensor for new data (calls read() internally)
        virtual bool update()
        {
            return read();
        }
        // Initializes the sensor and sets up any necessary parameters (calls init() internally)
        virtual bool begin()
        {
            return initialized = init();
        }

        virtual const SensorType getType() const { return type; }        // Returns the type of the sensor
        virtual const char *getTypeString() const { return typeString; } // Returns the type of the sensor as a string

        // ------------------------------- BASE SENSOR CLASS IMPLEMENTATION ----------------------------------------
    public:
        virtual bool isInitialized() const { return initialized; } // Returns whether the sensor has been initialized or not

        virtual explicit operator bool() const { return initialized; } // Returns whether the sensor has been initialized or not

        // ----------------------------------------------------------------------------------------------------------

    protected:
        // --------------------------------- HARDWARE IMPLEMENTATION -----------------------------------------------

        Sensor(const char *type, const char *name = nullptr) : DataReporter(name)
        {
            this->type = fnv1a_32(type, strlen(type));
            typeString = type;
        }
        // Sets up the sensor and stores any critical parameters. Needs to reset the sensor if it is already initialized. Called by begin()
        virtual bool init() = 0;
        // Physically reads the outputs from the sensor hardware. Called by update()
        virtual bool read() = 0;

        // ----------------------------------------------------------------------------------------------------------
        bool initialized = false;

        SensorType type;
        const char *typeString;

        // ----------------------------------------------------------------------------------------------------------
    };
}; // namespace mmfs

#endif // SENSOR_H