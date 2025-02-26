//
// Created by ramykaddouri on 10/22/24.
// Modified by michael mallamaci on 2/26/25 for reading from SD card
//

#include "SdDataReader.h"

using namespace mmfs
{
SdDataReader::SdDataReader(const std::filesystem::path &filePath) : filePath(filePath), fileStream(filePath) {
    if (!sd.begin(SD_CS_PIN, SD_SCK_MHZ(50))) {
        getLogger().recordLogData(ERROR_, "SdDataReader: SD initialization failed!");
        initialized = false;
        return;
    }

    if (!file.open(filePath, O_RDONLY)) {
        getLogger().recordLogData(WARNING_, "SdDataReader: Failed to open file: " + String(filePath));
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

        std::istringstream headerStream(line);
        String col;
        numCols = 0;

        while (std::getline(headerStream, col, ',')) {
            colNames[numCols++] = col;
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

    std::istringstream lineStream(line);
    size_t i = 0;
    String col;

    while (std::getline(lineStream, col, ',') && i < MAX_NUM_COLS) {
        data[i++] = static_cast<float>(strtod(col.c_str(), nullptr));
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
}