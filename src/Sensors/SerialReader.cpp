//
// Created by ramykaddouri on 10/22/24.
// Modified by michael mallamaci on 2/26/25 for reading from SD card
// Modified by michael mallamaci on 3/7/25 for reading from serial
//

#include "SerialReader.h"

namespace mmfs
{
SerialReader::SerialReader(const char* dataPointer) {
    initialized = true;
    dataPointer = dataPointer;
    getLogger().recordLogData(INFO_, "SerialReader: Initialized");
}

bool SerialReader::readColumnHeaders(int &numCols, String colNames[]) {
    if (lineIdx == 0) {
        String line(dataPointer);
        if (line.charAt(line.length() - 1) != ',') line += ",";

        numCols = 0;
        int startIdx = 0;
        for (int i = 0; i < line.length(); ++i) {
            if (line[i] == ',') {
                colNames[numCols++] = line.substring(startIdx, i);
                startIdx = i + 1;
            }
        }

        lineIdx++;
        return true;
    } else {
        getLogger().recordLogData(WARNING_, "SerialReader: Headers already read!");
        return false;
    }
}

bool SerialReader::readLine(float *data) {
    String line(dataPointer);
    if (line.length() == 0) return false;
    if (line.charAt(line.length() - 1) != ',') line += ",";

    int startIdx = 0;
    size_t i = 0;
    while (i < MAX_NUM_COLS) {
        int commaIdx = line.indexOf(',', startIdx);
        if (commaIdx == -1) break;
        String col = line.substring(startIdx, commaIdx);
        data[i++] = static_cast<float>(strtod(col.c_str(), nullptr));
        startIdx = commaIdx + 1;
    }

    lineIdx++;
    return i > 0;
}

bool SerialReader::isInit() const {
    return initialized;
}
}