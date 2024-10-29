//
// Created by ramykaddouri on 10/22/24.
//

#ifndef LAUNCHDATAREADER_H
#define LAUNCHDATAREADER_H
#include <filesystem>
#include <fstream>
#define MAX_NUM_COLS 1024

class LaunchDataReader {
private:
    bool initialized = false;
    size_t lineIdx = 0;
    const std::filesystem::path& filePath;  //fs::path is cross-platform,
                                            //should be easier to work with
    std::ifstream fileStream;
public:
    LaunchDataReader(const std::filesystem::path& filePath);
    bool isInit() const;

    //reads header of the CSV, sets numCols to number of columns and colNames
    //returns true if successful, false otherwise
    bool readColumnHeader(int& numCols, std::string colNames[]);

    //reads next line in CSV, sets data to be an array of sensor observations
    //the order is consistent with colNames in read_column_header
    //returns true if successful, false otherwise
    bool readLine(float* data);

    //close file stream
    void close();

    ~LaunchDataReader();
};



#endif //LAUNCHDATAREADER_H
