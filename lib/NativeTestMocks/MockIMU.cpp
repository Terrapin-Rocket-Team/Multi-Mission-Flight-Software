//
// Created by ramykaddouri on 11/1/24.
//

#include "MockIMU.h"

#include <iostream>

MockIMU::MockIMU(const std::filesystem::path &dataPath, const std::string accColNames[3],
                 const std::string gyroColNames[3], const std::string magColNames[3]) : dataReader(dataPath) {
    Sensor::setName("MockIMU");
    for(int i = 0; i < 3; i++) {
        this->accColNames[i] = accColNames[i];
        this->gyroColNames[i] = gyroColNames[i];
        this->magColNames[i] = magColNames[i];
    }
}

bool MockIMU::init() {
    if(!dataReader.isInit()) return false;

    int numCols = -1;
    std::string colNames[MAX_NUM_COLS];
    dataReader.readColumnHeaders(numCols, colNames);

    if(numCols == -1 || numCols > MAX_NUM_COLS) {
        std::cerr << "[MockIMU]: Invalid number of columns read: " << numCols << std::endl;
        return false;
    }

    for(int i = 0; i < numCols; i++) {
        for(int j = 0; j < 3; j++) {
            if(colNames[i] == accColNames[j]) {
                accIndices[j] = i;
            } else if(colNames[i] == gyroColNames[j]) {
                gyroIndices[j] = i;
            } else if(colNames[i] == magColNames[j]) {
                magIndices[j] = i;
            }
        }
    }

    for(int i = 0; i < 3; i++) {
        if(accIndices[i] == -1) {
            std::cerr << "[MockIMU]: Failed to find acceleration column index for name: " << accColNames[i] << std::endl;
            return false;
        }

        if(gyroIndices[i] == -1) {
            std::cerr << "[MockIMU]: Failed to find gyroscope column index for name: " << gyroColNames[i] << std::endl;
            return false;
        }

        if(magIndices[i] == -1) {
            std::cerr << "[MockIMU]: Failed to find magnetometer column index for name: " << magColNames[i] << std::endl;
            return false;
        }
    }

    return true;
}

void MockIMU::read() {
    for(int i = 0; i < 3; i++) {
        measuredAcc[i] = launchData[accIndices[i]];
        measuredGyro[i] = launchData[gyroIndices[i]];
        measuredMag[i] = launchData[magIndices[i]];
    }
}

MockIMU::~MockIMU() {
    dataReader.close();
}
