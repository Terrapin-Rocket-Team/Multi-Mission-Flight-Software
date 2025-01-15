#ifndef DATA_REPORTER_H
#define DATA_REPORTER_H

#include <stdint.h>
#include <cstdio>
#include <cstring>
#include "Logger.h"

namespace mmfs
{

    enum PackedType
    {
        INT,
        BYTE,
        SHORT,
        FLOAT,
        DOUBLE, // Compressed to a FLOAT
        DOUBLE_HP, // Remains a double after compression
        STRING,
        BOOL,
        LONG,
        UNKNOWN
    };

    struct PackedInfo
    {
        PackedType type = UNKNOWN;   // Type of the data
        const char *label = nullptr; // Label for the data
        PackedInfo *next = nullptr;  // Next packed info
        void *data = nullptr;        // Pointer to the data the user gave
        int8_t size = -1;            // Optional for floats/doubles/strings
    };

    class DataReporter
    {
    public:
        static int numReporters; // Only incremented throughout the life of the program

        DataReporter(const char *name = nullptr);
        virtual ~DataReporter();

        virtual const char *getName() const;
        virtual void setName(const char *n);

        uint8_t *getPackedData();
        int getPackedDataSize();
        int getNumColumns();
        PackedInfo *getPackedInfo();

        virtual void packData();

    protected:
        virtual void initializeDataReporting();

        uint8_t *packedData = nullptr; // Packed data for RecordData
        int packedDataSize = 0;        // Size of packedData in bytes
        uint8_t numColumns = 0;        // Number of fields in packedData

        PackedInfo *first = nullptr, *last = nullptr;

        template <typename T>
        void insertColumn(uint8_t place, PackedType t, T *variable, const char *label);

        template <typename T>
        void addColumn(PackedType t, T *variable, const char *label);

        // void removeColumn(const char *label);

    private:
        char *name = nullptr; // Name of the object

        static uint8_t findSizeOfType(PackedType t, void *str);
    };

} // namespace mmfs

#include "DataReporter.inl" // Include templates implementation
#endif                      // DATA_REPORTER_H
