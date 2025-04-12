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
        // get current data or pass in a pointer to a different data array
        static void toCSVRow(char *dest, int destLen, DataReporter **drs, int numDrs, void *data = nullptr);

        // get the header for the CSV file
        static void getCSVHeader(char *dest, int destLen, DataReporter **drs, int numDrs);

    private:
        static char *toCSVSection(char *dest, int &destLen, DataReporter *d);
    };
}

#endif // DATAFORMATTER_H