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
        static uintptr_t packData(uintptr_t dest, State *state); // returns the next available memory location

        // get current data or pass in a pointer to a different data array
        static void toCSVRow(char *dest, int destLen, State *state, void *date = nullptr);

        // get the header for the CSV file
        static void getCSVHeader(char *dest, int destLen, State *state);

    private:
        static uintptr_t toCSVSection(char *dest, int &destLen, void *data, int &dataOffset, DataReporter *d);
    };
}

#endif // DATAFORMATTER_H