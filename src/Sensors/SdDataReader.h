
//
// Created by ramykaddouri on 10/22/24.
// Modified by michael mallamaci on 2/26/25 for reading from SD card
//

#ifndef SDDATAREADER_H
#define SDDATAREADER_H
#include "SdFat.h"
#include "../RecordData/SdFatBoilerplate.h"
#include "../RecordData/Logger.h"
#include <Arduino.h>
#define MAX_NUM_COLS 1024

using namespace mmfs;

class SdDataReader {
    private:
        bool initialized = false;
        size_t lineIdx = 0;
        const char* filePath;
        SdFat sd;
        SdFile file;
        char buffer[256]; // Buffer to read lines from the file TODO increase
    
    public:
        SdDataReader(const char* filePath);
        bool isInit() const;
    
        //reads header of the CSV, sets numCols to number of columns and colNames
        //returns true if successful, false otherwise
        bool readColumnHeaders(int& numCols, String colNames[]);
    
        //reads next line in CSV, sets data to be an array of sensor observations
        //the order is consistent with colNames in read_column_header
        //returns true if successful, false otherwise
        bool readLine(float* data);
    
        //close file stream
        void close();
    
        ~SdDataReader();
    };

#endif //SDDATAREADER_H
