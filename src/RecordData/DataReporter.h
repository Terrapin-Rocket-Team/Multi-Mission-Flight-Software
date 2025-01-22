#ifndef DATA_REPORTER_H
#define DATA_REPORTER_H

#include <stdint.h>

namespace mmfs
{

    enum PackedType : uint8_t
    { // These are the types of data that can be stored in the sensor's data array
        BOOL,
        BYTE,
        SHORT,
        INT,
        LONG,
        FLOAT,
        DOUBLE,
        STRING_10,
        STRING_20,
        STRING_50,
    };

    class DataReporter
    {

    public:
        virtual ~DataReporter()
        {
            delete[] packedData;
        };
        virtual const char *getName() const { return name; } // Returns the name of the sensor
        virtual void setName(const char *n)                  // Sets the name of the sensor
        {
            delete[] name;
            int len = strlen(n);
            name = new char[len + 1];
            snprintf(name, len + 1, "%s", n);
        }

        // User must implement these functions
        virtual const PackedType *getPackedOrder() const = 0; // Sets the order of the data types in packedData
        virtual const int getNumPackedDataPoints() const = 0; // Returns the number of data points in packedData
        virtual const char **getPackedDataLabels() const = 0; // returns string labels for each data point, in order.

        // User may implement these functions
        virtual uint8_t *getPackedData() const { return packedData; }          // Returns the packed data for RecordData
        virtual const int getPackedDataSize() const { return packedDataSize; } // Returns the size of the packed data

    protected:
        // User must implement this function
        virtual void packData() = 0; // Packs the data into packedData

        virtual void setUpPackedData()
        {
            delete[] packedData;
            int size = 0;
            for (int i = 0; i < getNumPackedDataPoints(); i++)
                size += PackedTypeToSize(getPackedOrder()[i]);

            packedData = new uint8_t[size];
            packedDataSize = size;
        }

        uint8_t *packedData = nullptr; // Packed data for RecordData
        int packedDataSize = 0;        // Size of packedData
        char *name = nullptr;          // Name of the object

    public:
        static const int PackedTypeToSize(const PackedType &type)
        {
            switch (type)
            {
            case BOOL:
                return sizeof(float);
            case BYTE:
                return sizeof(float);
            case SHORT:
                return sizeof(float);
            case INT:
                return sizeof(float);
            case LONG:
                return sizeof(long int);
            case FLOAT:
                return sizeof(float);
            case DOUBLE:
                return sizeof(double);
            case STRING_10:
                return 12;
            case STRING_20:
                return 20;
            case STRING_50:
                return 52;
            default:
                return 0;
            }
        }
    };
} // namespace mmfs
#endif // DATA_REPORTER_H