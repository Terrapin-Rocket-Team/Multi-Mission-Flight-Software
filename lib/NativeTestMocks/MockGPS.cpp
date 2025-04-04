//
// Created by ramykaddouri on 11/1/24.
//

#include "MockGPS.h"

#include <iostream>

MockGPS::MockGPS(const std::filesystem::path &dataPath,
                 const std::string &posXColName,
                 const std::string &posYColName,
                 const std::string &posZColName,
                 const std::string &headingColName,
                 const std::string &fixQualityColName) :
    dataReader(dataPath),
    posXColName(posXColName),
    posYColName(posYColName),
    posZColName(posZColName),
    headingColName(headingColName),
    fixQualityColName(fixQualityColName) {
    Sensor::setName("MockGPS");
}

bool MockGPS::init() {

    if(!dataReader.isInit()) return false;

    int numCols = -1;
    std::string colNames[MAX_NUM_COLS];
    dataReader.readColumnHeaders(numCols, colNames);

    if(numCols == -1 || numCols > MAX_NUM_COLS) {
        std::cerr << "[MockGPS]: Invalid number of columns read: " << numCols << std::endl;
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
        posXColIdx = MAX_NUM_COLS-1;
    }
    if(posYColName == "_") {
        posYColIdx = MAX_NUM_COLS-1;
    }
    if(posZColName == "_") {
        posZColIdx = MAX_NUM_COLS-1;
    }
    if(headingColName == "_") {
        headingColIdx = MAX_NUM_COLS-1;
    }
    if(fixQualityColName == "_") {
        fixQualityColIdx = MAX_NUM_COLS-1;
    }

    if(posXColIdx == -1) {
        std::cerr << "[MockGPS]: Failed to find posX column index for name: " << posXColName << std::endl;
        return false;
    }
    if(posYColIdx == -1) {
        std::cerr << "[MockGPS]: Failed to find pozY column index for name: " << posYColName << std::endl;
        return false;
    }
    if(posZColIdx == -1) {
        std::cerr << "[MockGPS]: Failed to find posZ column index for name: " << posZColName << std::endl;
        return false;
    }
    if(headingColIdx == -1) {
        std::cerr << "[MockGPS]: Failed to find heading column index for name: " << headingColName << std::endl;
        return false;
    }
    if(fixQualityColIdx == -1) {
        std::cerr << "[MockGPS]: Failed to find fix quality column index for name: " << fixQualityColName << std::endl;
        return false;
    }

    initialized = true;

    return true;
}

void MockGPS::read() {
    if(!dataReader.readLine(launchData)) {
        std::cerr << "[MockGPS]: Failed to read data from file!" << std::endl;
        initialized = false;
        return;
    }
    position.x() = launchData[posXColIdx];
    position.y() = launchData[posYColIdx];
    position.z() = launchData[posZColIdx];


    heading = launchData[headingColIdx];
    fixQual = static_cast<int>(launchData[fixQualityColIdx]);
}

mmfs::Vector<3> MockGPS::getPos() const {
    return GPS::getPos();
}

double MockGPS::getHeading() const {
    return GPS::getHeading();
}

int MockGPS::getFixQual() const {
    return GPS::getFixQual();
}


MockGPS::~MockGPS() {
    dataReader.close();
}
