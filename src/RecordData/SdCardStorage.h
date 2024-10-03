#ifndef SD_CARD_STORAGE_H
#define SD_CARD_STORAGE_H

#include "StorageType.h"
#include "SdFat.h"
#include <cstring> // For strncpy and strcmp
#include <Arduino.h>

namespace mmfs {

extern SdFs sd;
extern FsFile logFile;
extern FsFile flightDataFile;


// called this to avoid naming conflicts
// could always reference with mmfs::SdCard,
// but this is more convenient if you're using
// `using namespace mmfs;`
class SdCardStorage : public StorageFile
{
private:
    static constexpr int NAME_SIZE = 24;

    struct FileNode {
        FsFile file;
        char fileName[NAME_SIZE];
        uint64_t offset;
        FileNode* next;
        int index;
    };

    FileNode* head;
    FileNode* currentFileNode;
    bool sdReady;

public:
    SdFs sd;
    int fileIndex;

    SdCardStorage();
    ~SdCardStorage();

    bool init() override;
    bool init(const char *fileSuffix);

    bool isReady() const override;

    void write(char *data, int size) override;
    void print(const char *data);
    void println(const char *data);

    int read(char *data, int size) override;
    int readTo(char *data, char endChar) override;

    bool seek(uint64_t offset) override;

    bool addFile(const char* fileName);
    bool selectFile(const char* fileName);
    bool selectFile(int index);
    bool selectNextFile();
};

} // namespace mmfs

#endif // SD_CARD_STORAGE_H
