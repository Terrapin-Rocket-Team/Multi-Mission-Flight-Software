//
// Created by ramykaddouri on 10/22/24.
//

#include "LaunchDataReader.h"

#include <cstring>
#include <iostream>

LaunchDataReader::LaunchDataReader(const std::filesystem::path &filePath) : filePath(filePath), fileStream(filePath) {
    if(!fileStream.is_open()) {
        std::cerr << "LaunchDataReader: Failed to open data file at " << filePath << std::endl;
        initialized = false;
    } else {
        lineIdx = 0;
        initialized = true;
    }
}

bool LaunchDataReader::read_column_header(int &numCols, std::string colNames[]) {
    if(lineIdx == 0) {
        std::string line;
        std::getline(fileStream, line);

        if(line[line.size()] != ',') line+=",";

        std::istringstream headerStream = std::istringstream(line);

        std::string col;
        numCols = 0;
        while(std::getline(headerStream, col, ',').good()) {
            // std::cout << "Column " << numCols << ": " << col << std::endl;
            colNames[numCols] = col;
            numCols++;
        }

        lineIdx++;
        return true;
    } else {
        std::cerr << "LaunchDataReader::read_column_header: Line index is not 0. This function must be called before any reads!" << std::endl;
        return false;
    }
}

bool LaunchDataReader::read(float *data) {
    std::string line;
    std::getline(fileStream, line);

    if(!fileStream.good()) return false;

    std::istringstream lineStream = std::istringstream(line);

    std::string col;
    size_t i = 0;
    while(lineStream.good()) {
        float measurement;
        lineStream >> measurement;
        data[i] = measurement;
        i++;
    }

    lineIdx++;

    return i > 0;
}


bool LaunchDataReader::is_initialized() const {
    return initialized;
}

LaunchDataReader::~LaunchDataReader() {
    fileStream.close();
}

