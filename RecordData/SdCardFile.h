#ifndef SD_CARD_FILE_H
#define SD_CARD_FILE_H

#include "StorageType.h"
#include "SdFat.h"

#ifndef SD_FAT_TYPE
#define SD_FAT_TYPE 3
#endif

#pragma region SD configs
/*
  Change the value of SD_CS_PIN if you are using SPI and
  your hardware does not use the default value, SS.
  Common values are:
  Arduino Ethernet shield: pin 4
  Sparkfun SD shield: pin 8
  Adafruit SD shields and modules: pin 10
*/

// SDCARD_SS_PIN is defined for the built-in SD on some boards.
#ifndef SDCARD_SS_PIN
const uint8_t SD_CS_PIN = SS;
#else  // SDCARD_SS_PIN
// Assume built-in SD is used.
const uint8_t SD_CS_PIN = SDCARD_SS_PIN;
#endif // SDCARD_SS_PIN

// Try max SPI clock for an SD. Reduce SPI_CLOCK if errors occur.
#define SPI_CLOCK SD_SCK_MHZ(50)

// Try to select the best SD card configuration.
#if HAS_SDIO_CLASS
#define SD_CONFIG SdioConfig(FIFO_SDIO)
#elif ENABLE_DEDICATED_SPI
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SPI_CLOCK)
#else // HAS_SDIO_CLASS
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, SHARED_SPI, SPI_CLOCK)
#endif // HAS_SDIO_CLASS

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
#else // SD_FAT_TYPE
#error Invalid SD_FAT_TYPE
#endif // SD_FAT_TYPE

#pragma endregion

class SdCardFile : public StorageType
{
protected:
    static constexpr int NAME_SIZE = 24;
    FsFile file;
    SdFs sd;
public:
    char fileName[NAME_SIZE];

    bool sdReady;

    SdCardFile();
    bool init();
    bool init(const char *fileSuffix);
    bool isReady() const override;
    void write(char *data, int size) override;
    void print(const char *data);
    void println(const char *data);
    int read(char *data, int size) override;
    int readTo(char *data, char endChar) override;
    bool seek(uint64_t offset) override;
};


#endif //SD_CARD_FILE_H