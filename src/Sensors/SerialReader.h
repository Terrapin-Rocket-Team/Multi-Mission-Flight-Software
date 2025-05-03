#ifndef SERIALREADER_H
#define SERIALREADER_H
#include "../RecordData/Logging/Logger.h"
#include <Arduino.h>
#include <string>
#define MAX_NUM_COLS 1024

namespace mmfs
{
class SerialReader {
    private:
        bool initialized = false;
        size_t lineIdx = 0;
    
    public:
        SerialReader(const char* dataPointer);
        bool isInit() const;
        const char* dataPointer;
    
        //reads header of the CSV, sets numCols to number of columns and colNames
        //returns true if successful, false otherwise
        bool readColumnHeaders(int& numCols, std::string colNames[]);
    
        //reads next line in CSV, sets data to be an array of sensor observations
        //the order is consistent with colNames in read_column_header
        //returns true if successful, false otherwise
        bool readLine(float* data);
    
        ~SerialReader(){};
    };
}

#endif //SERIALREADER_H
