//
// Created by ramykaddouri on 10/22/24.
// Modified by michael mallamaci on 2/26/25 for reading from SD card
//

#include "SdDataReader.h"

using namespace mmfs;

SdDataReader::SdDataReader(const char* filePath) {
    if (!sd.begin(SD_CS_PIN, SD_SCK_MHZ(50))) {
        getLogger().recordLogData(ERROR_, "SdDataReader: SD initialization failed!");
        initialized = false;
        return;
    }

    if (!file.open(filePath, O_RDONLY)) {
        getLogger().recordLogData(WARNING_, "SdDataReader: Failed to open file: " + String(filePath).c_str());
        initialized = false;
        return;
    }
    
    getLogger().recordLogData(INFO_, "SdDataReader: File opened successfully!");
    initialized = true;
}

bool SdDataReader::readColumnHeaders(int &numCols, String colNames[]) {
    if (lineIdx == 0) {
        if (!file.fgets(buffer, sizeof(buffer))) {
            getLogger().recordLogData(WARNING_, "SdDataReader: Failed to read column headers!");
            return false;
        }

        String line(buffer);
        if (line.back() != ',') line += ",";

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
        getLogger().recordLogData(WARNING_, "SdDataReader: Headers already read!");
        return false;
    }
}

bool SdDataReader::readLine(float *data) {
    if (!file.fgets(buffer, sizeof(buffer))) return false;

    String line(buffer);
    if (line.empty()) return false;
    if (line.back() != ',') line += ",";

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

bool SdDataReader::isInit() const {
    return initialized;
}

void SdDataReader::close() {
    file.close();
    getLogger().recordLogData(INFO_, "SdDataReader: File closed.");
}

SdDataReader::~SdDataReader() {
    close();
}