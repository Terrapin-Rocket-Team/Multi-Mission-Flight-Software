//
// Created by ramykaddouri on 10/22/24.
//

#ifndef LAUNCHDATAREADER_H
#define LAUNCHDATAREADER_H
#include <filesystem>
#include <fstream>


class LaunchDataReader {
private:
    bool initialized = false;
    size_t lineIdx = 0;
    const std::filesystem::path& filePath;  //fs::path is cross-platform,
                                            //should be easier to work with
    std::ifstream fileStream;
public:
    LaunchDataReader(const std::filesystem::path& filePath);
    bool is_initialized() const;

    //reads header of the CSV, sets numCols to number of columns and colNames
    //returns true if successful, false otherwise
    bool read_column_header(int& numCols, char** colNames);

    //reads next line in CSV, sets data to be an array of sensor observations
    //the order is consistent with colNames in read_column_header
    //returns true if successful, false otherwise
    bool read(float* data);

    ~LaunchDataReader();
};



#endif //LAUNCHDATAREADER_H
