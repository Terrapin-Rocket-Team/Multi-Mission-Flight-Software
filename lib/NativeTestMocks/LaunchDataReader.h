//
// Created by ramykaddouri on 10/22/24.
//

#ifndef LAUNCHDATAREADER_H
#define LAUNCHDATAREADER_H
#include <filesystem>
#include <string>


class LaunchDataReader {
private:
    bool initialized = false;
    size_t lineIdx;
    const std::filesystem::path& filePath;  //fs::path is cross-platform,
                                            //should be easier to work with
    std::iostream<char> fileStream;
    size_t numColumns;
public:
    LaunchDataReader(const std::filesystem::path& filePath);
    bool is_initialized() const;
    ~LaunchDataReader();
};



#endif //LAUNCHDATAREADER_H
