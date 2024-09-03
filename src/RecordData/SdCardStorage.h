#ifndef SD_CARD_H
#define SD_CARD_H

#include "StorageType.h"
#include "SdFat.h"
#include <cstring> // For strncpy and strcmp

#ifndef SD_FAT_TYPE
#define SD_FAT_TYPE 3
#endif

namespace mmfs {

#pragma region SD configs
#ifndef SDCARD_SS_PIN
const uint8_t SD_CS_PIN = SS;
#else
const uint8_t SD_CS_PIN = SDCARD_SS_PIN;
#endif

#define SPI_CLOCK SD_SCK_MHZ(50)

#if HAS_SDIO_CLASS
#define SD_CONFIG SdioConfig(FIFO_SDIO)
#elif ENABLE_DEDICATED_SPI
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SPI_CLOCK)
#else
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, SHARED_SPI, SPI_CLOCK)
#endif

#if SD_FAT_TYPE == 0
SdFat sd;
File logFile;
File flightDataFile;
#elif SD_FAT_TYPE == 1
SdFat32 sd;
File32 logFile;
File32 flightDataFile;
#elif SD_FAT_TYPE == 2
SdExFat sd;
ExFile logFile;
ExFile flightDataFile;
#elif SD_FAT_TYPE == 3
SdFs sd;
FsFile logFile;
FsFile flightDataFile;
#else
#error Invalid SD_FAT_TYPE
#endif

#pragma endregion

// called this to avoid naming conflicts
// could always reference with mmfs::SdCard,
// but this is more convenient if you're using
// `using namespace mmfs;`
class SdCardStorage : public StorageType
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

#endif // SD_CARD_H
