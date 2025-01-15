#ifndef DATAFORMATTER_H
#define DATAFORMATTER_H

#include <stdint.h>

namespace mmfs
{
    class State;
    class DataReporter;

    class DataFormatter
    {
    public:
        static uint8_t *packData(uint8_t *dest, DataReporter **drs, int numDrs); // returns the next available memory location

        // get current data or pass in a pointer to a different data array
        static void toCSVRow(char *dest, int destLen, DataReporter **drs, int numDrs, void *data = nullptr);

        // get the header for the CSV file
        static void getCSVHeader(char *dest, int destLen, DataReporter **drs, int numDrs);

        static int getPackedLen(DataReporter **drs, int numDrs);

    private:
        static char *toCSVSection(char *dest, int &destLen, void *data, int &dataOffset, DataReporter *d);
    };
}

#endif // DATAFORMATTER_H