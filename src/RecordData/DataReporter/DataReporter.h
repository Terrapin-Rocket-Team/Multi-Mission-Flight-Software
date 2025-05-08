#ifndef DATA_REPORTER_H
#define DATA_REPORTER_H

#include <stdint.h>
#include <cstdio>
#include <cstring>
#include "../Logging/Logger.h"

namespace mmfs
{

    enum DataType
    {
        INT,
        BYTE,
        SHORT,
        FLOAT,
        DOUBLE,    // Displays with 3 decimal places
        DOUBLE_HP, // Displays with 7 decimal places
        STRING,
        BOOL,
        LONG,
        UNKNOWN
    };

    struct DataPoint
    {
        DataType type = UNKNOWN;     // Type of the data
        const char *label = nullptr; // Label for the data
        DataPoint *next = nullptr;   // Next data point in the list
        void *data = nullptr;        // Pointer to the data the user gave
    };

    class DataReporter
    {
    public:
        static int numReporters; // Only incremented throughout the life of the program, used to assign new unique names

        DataReporter(const char *name = nullptr);
        virtual ~DataReporter();

        virtual const char *getName() const;
        virtual void setName(const char *n);

        int getNumColumns();
        DataPoint *getDataPoints();

    protected:
        uint8_t numColumns = 0;

        DataPoint *first = nullptr, *last = nullptr;

        template <typename T>
        void insertColumn(uint8_t place, DataType t, T *variable, const char *label);

        template <typename T>
        void addColumn(DataType t, T *variable, const char *label);

        void removeColumn(const char *label);

    private:
        char *name = nullptr; // Name of the object
    };

} // namespace mmfs

#include "DataReporter.inl" // Include templates implementation
#endif                      // DATA_REPORTER_H
