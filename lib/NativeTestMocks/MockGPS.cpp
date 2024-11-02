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
    fixQualityColName(fixQualityColName) {}

bool MockGPS::init() {

    if(!dataReader.isInit()) return false;

    int numCols = -1;
    std::string colNames[MAX_NUM_COLS];
    dataReader.readColumnHeader(numCols, colNames);

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

    return true;
}

void MockGPS::read() {
    position.x() = launchData[posXColName];
    position.y() = launchData[posYColName];
    position.z() = launchData[posZColName];

    heading = launchData[headingColName];
    fixQual = launchData[fixQualityColName];
}

MockGPS::~MockGPS() {
    dataReader.close();
}



