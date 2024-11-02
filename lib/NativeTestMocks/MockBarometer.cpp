//
// Created by ramykaddouri on 10/29/24.
//

#include "MockBarometer.h"

#include <iostream>

MockBarometer::MockBarometer(const std::filesystem::path &dataPath, const std::string& pressureColName, const std::string& temperatureColName) :
    dataReader(dataPath), pressureColName(pressureColName), temperatureColName(temperatureColName) {
    Sensor::setName("MockBarometer");
}

bool MockBarometer::init() {
    // std::cout << "pressureColName: " << pressureColName << std::endl;
    // std::cout << "tempColName: " << temperatureColName << std::endl;
    if(!dataReader.isInit()) return false;

    int numCols = -1;
    std::string colNames[MAX_NUM_COLS];
    dataReader.readColumnHeader(numCols, colNames);

    if(numCols == -1 || numCols > MAX_NUM_COLS) {
        std::cerr << "[MockBarometer]: Invalid number of columns read: " << numCols << std::endl;
        return false;
    }

    for(int i = 0; i < numCols; i++) {
        if(colNames[i] == pressureColName) {
            pressureColIndex = i;
        } else if(colNames[i] == temperatureColName) {
            temperatureColIndex = i;
        }
    }

    if(pressureColIndex == -1) {
        std::cerr << "[MockBarometer]: Failed to find pressure column index for name: " << pressureColName << std::endl;
        return false;
    }
    if(temperatureColIndex == -1) {
        std::cerr << "[MockBarometer]: Failed to find temperature column index for name: " << pressureColName << std::endl;
        return false;
    }

    return true;
}

void MockBarometer::read() {
    dataReader.readLine(launchData);
    std::cout << "pressure: " << launchData[pressureColIndex] << std::endl;
    std::cout << "temp: " << launchData[temperatureColIndex] << std::endl;

    pressure = launchData[pressureColIndex];
    temp = launchData[temperatureColIndex];
}

MockBarometer::~MockBarometer() {
    dataReader.close();
}

