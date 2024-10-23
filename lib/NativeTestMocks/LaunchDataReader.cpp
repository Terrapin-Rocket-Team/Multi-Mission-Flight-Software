//
// Created by ramykaddouri on 10/22/24.
//

#include "LaunchDataReader.h"

#include <iostream>

LaunchDataReader::LaunchDataReader(const std::filesystem::path &filePath) : filePath(filePath), fileStream(filePath) {
    if(!fileStream.is_open()) {
        std::cerr << "LaunchDataReader: Failed to open data file at " << filePath << std::endl;
        initialized = false;
    }
}

bool LaunchDataReader::is_initialized() const {
    return initialized;
}

LaunchDataReader::~LaunchDataReader() {
    fileStream.close();
}

