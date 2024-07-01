#ifndef SD_CARD_FILE_H
#define SD_CARD_FILE_H

#include "StorageType.h"
#include "SdFat.h"

#ifndef SD_FAT_TYPE
#define SD_FAT_TYPE 3
#endif

namespace mmfs {

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

    /**
     * Constructor for SdCardFile.
     */
    SdCardFile();

    /**
     * Initialize the SD card.
     * @return true if initialization was successful, false otherwise.
     */
    bool init() override;

    /**
     * Initialize the SD card with a specific file suffix.
     * @param fileSuffix The suffix for the file name.
     * @return true if initialization was successful, false otherwise.
     */
    bool init(const char *fileSuffix);

    /**
     * Check if the SD card is ready for read/write operations.
     * @return true if the SD card is ready, false otherwise.
     */
    bool isReady() const override;

    /**
     * Write data to the SD card.
     * @param data The data to be written.
     * @param size The size of the data to be written.
     */
    void write(char *data, int size) override;

    /**
     * Print a string to the SD card.
     * @param data The string to be printed. Takes a pointer to a null-terminated character array.
     */
    void print(const char *data);

    /**
     * Print a string to the SD card followed by a newline.
     * @param data The string to be printed. Takes a pointer to a null-terminated character array.
     */
    void println(const char *data);

    /**
     * Read data from the SD card.
     * @param data The buffer to store the read data.
     * @param size The size of the data to be read.
     * @return The number of bytes read. If unsuccessful, returns -1.
     */
    int read(char *data, int size) override;

    /**
     * Read data from the SD card until a specified end character is encountered.
     * @param data The buffer to store the read data.
     * @param endChar The character to stop reading at.
     * @return The number of bytes read. If unsuccessful, returns -1.
     */
    int readTo(char *data, char endChar) override;

    /**
     * Seek to a specific offset in the SD card.
     * @param offset The offset to seek to. Relative to the beginning of the file.
     * @return true if the seek operation was successful, false otherwise.
     */
    bool seek(uint64_t offset) override;
};

} // namespace mmfs

#endif //SD_CARD_FILE_H