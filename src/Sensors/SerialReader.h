
//
// Created by ramykaddouri on 10/22/24.
// Modified by michael mallamaci on 2/26/25 for reading from SD card
// Modified by michael mallamaci on 3/7/25 for reading from serial
//

#ifndef SERIALREADER_H
#define SERIALREADER_H
#include "../RecordData/Logging/Logger.h"
#include <Arduino.h>
#define MAX_NUM_COLS 1024

namespace mmfs
{
class SerialReader {
    private:
        bool initialized = false;
        size_t lineIdx = 0;
        const char* dataPointer;
    
    public:
        SerialReader(const char* dataPointer);
        bool isInit() const;
    
        //reads header of the CSV, sets numCols to number of columns and colNames
        //returns true if successful, false otherwise
        bool readColumnHeaders(int& numCols, String colNames[]);
    
        //reads next line in CSV, sets data to be an array of sensor observations
        //the order is consistent with colNames in read_column_header
        //returns true if successful, false otherwise
        bool readLine(float* data);
    
        ~SerialReader(){};
    };
}

#endif //SERIALREADER_H
