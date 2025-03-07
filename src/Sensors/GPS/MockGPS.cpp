//
// Created by ramykaddouri on 11/1/24.
// Modified by michael mallamaci on 3/1/25 for reading from SD card
//

#include "MockGPS.h"

namespace mmfs
{
MockGPS::MockGPS(const char* dataPath,
                 const String& posXColName,
                 const String& posYColName,
                 const String& posZColName,
                 const String& headingColName,
                 const String& fixQualityColName) :
    dataReader(dataPath), posXColName(posXColName), posYColName(posYColName),
    posZColName(posZColName), headingColName(headingColName), fixQualityColName(fixQualityColName) {
    Sensor::setName("MockGPS");
}

bool MockGPS::init() {
    if(!dataReader.isInit()) return false;

    int numCols = -1;
    String colNames[MAX_NUM_COLS];
    dataReader.readColumnHeaders(numCols, colNames);

    if(numCols == -1 || numCols > MAX_NUM_COLS) {
        getLogger().recordLogData(ERROR_, 100, "[MockGPS]: Invalid number of columns read: %d", numCols);
        return false;
    }

    for(int i = 0; i < numCols; i++) {
        if(colNames[i] == posXColName) {
            posXColIdx = i;
        } else if(colNames[i] == posYColName) {
            posYColIdx = i;
        } else if(colNames[i] == posZColName) {
            posZColIdx = i;
        } else if(colNames[i] == headingColName) {
            headingColIdx = i;
        } else if(colNames[i] == fixQualityColName) {
            fixQualityColIdx = i;
        }
    }

    // underscores indicate skipped fields
    if(posXColName == "_") {
        posXColIdx = MAX_NUM_COLS - 1;
    }
    if(posYColName == "_") {
        posYColIdx = MAX_NUM_COLS - 1;
    }
    if(posZColName == "_") {
        posZColIdx = MAX_NUM_COLS - 1;
    }
    if(headingColName == "_") {
        headingColIdx = MAX_NUM_COLS - 1;
    }
    if(fixQualityColName == "_") {
        fixQualityColIdx = MAX_NUM_COLS - 1;
    }

    if(posXColIdx == -1) {
        getLogger().recordLogData(ERROR_, 100, "[MockGPS]: Failed to find posX column index for name: %s", posXColName.c_str());
        return false;
    }
    if(posYColIdx == -1) {
        getLogger().recordLogData(ERROR_, 100, "[MockGPS]: Failed to find posY column index for name: %s", posYColName.c_str());
        return false;
    }
    if(posZColIdx == -1) {
        getLogger().recordLogData(ERROR_, 100, "[MockGPS]: Failed to find posZ column index for name: %s", posZColName.c_str());
        return false;
    }
    if(headingColIdx == -1) {
        getLogger().recordLogData(ERROR_, 100, "[MockGPS]: Failed to find heading column index for name: %s", headingColName.c_str());
        return false;
    }
    if(fixQualityColIdx == -1) {
        getLogger().recordLogData(ERROR_, 100, "[MockGPS]: Failed to find fix quality column index for name: %s", fixQualityColName.c_str());
        return false;
    }

    initialized = true;
    return true;
}

void MockGPS::read() {
    if(!dataReader.readLine(sdData)) {
        getLogger().recordLogData(ERROR_, 100, "[MockGPS]: Failed to read data from file!");
        initialized = false;
        return;
    }
    position.x() = sdData[posXColIdx];
    position.y() = sdData[posYColIdx];
    position.z() = sdData[posZColIdx];

    heading = sdData[headingColIdx];
    fixQual = static_cast<int>(sdData[fixQualityColIdx]);
}

}  // namespace mmfs
