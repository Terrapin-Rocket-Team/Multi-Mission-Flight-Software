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
        virtual void update()
        {
            read();
        }
        // Initializes the sensor and sets up any necessary parameters (calls init() internally)
        virtual bool begin(bool useBiasCorrection = true)
        {
            return init();
        }

        virtual const SensorType getType() const { return type; }        // Returns the type of the sensor
        virtual const char *getTypeString() const { return typeString; } // Returns the type of the sensor as a string

        // ------------------------------- BASE SENSOR CLASS IMPLEMENTATION ----------------------------------------
    public:
        virtual bool isInitialized() const { return initialized; } // Returns whether the sensor has been initialized or not

        virtual explicit operator bool() const { return initialized; } // Returns whether the sensor has been initialized or not

        virtual void markLiftoff() { biasCorrectionMode = false; } // Sets the bias correction mode to false, storing origin positions

        virtual void setBiasCorrectionMode(bool mode) { biasCorrectionMode = mode; } // whether or not to continually re-zero self using incoming data.

        // ----------------------------------------------------------------------------------------------------------

    protected:
        // --------------------------------- HARDWARE IMPLEMENTATION -----------------------------------------------
        Sensor(const char *type)
        {
            this->type = fnv1a_32(type, strlen(type));
            typeString = type;
        }
        // Sets up the sensor and stores any critical parameters. Needs to reset the sensor if it is already initialized. Called by begin()
        virtual bool init() = 0;
        // Physically reads the outputs from the sensor hardware. Called by update()
        virtual void read() = 0;

        // ----------------------------------------------------------------------------------------------------------
        bool initialized = false;
        bool biasCorrectionMode = true;

        SensorType type;
        const char *typeString;

        const int CIRC_BUFFER_LENGTH = UPDATE_RATE * SENSOR_BIAS_CORRECTION_DATA_LENGTH; // number of entries to give SBCDL length average
        const int CIRC_BUFFER_IGNORE = UPDATE_RATE * SENSOR_BIAS_CORRECTION_DATA_IGNORE; // number of entries to ignore for SBCD

        // ----------------------------------------------------------------------------------------------------------
    };
}; // namespace mmfs

#endif // SENSOR_H