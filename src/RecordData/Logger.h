/**
 * @file Logger.h
 * @brief This file contains the declaration of the Logger class, an example use case of the PSRAM and SdCardFile classes.
 * Based on the mode, the Logger class will write data to the PSRAM or the SD card, writing to the SD card when the mode is GROUND
 * and writing to the PSRAM when the mode is FLIGHT. The Logger class will also write data to the USB serial port when connected if
 * the destination is set to TO_USB or BOTH, which it is by default.
 */

#ifndef LOGGER_H
#define LOGGER_H

// SD_FAT_TYPE = 0 for SdFat/File as defined in SdFatConfig.h,
// 1 for FAT16/FAT32, 2 for exFAT, 3 for FAT16/FAT32 and exFAT.
#define SD_FAT_TYPE 3

#include "psram.h"
#include "SdFat.h"
#include "Arduino.h"

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

namespace mmfs
{

    enum LogType
    {
        LOG_,
        ERROR_,
        WARNING_,
        INFO_
    };

    enum Dest
    {
        TO_FILE,
        TO_USB,
        BOTH
    };

    enum Mode
    {
        GROUND,
        FLIGHT
    };

    class Logger
    {
    private:
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

    public:
        Logger(uint16_t bufferSize = 25000, int bufferInterval = 300);
        virtual ~Logger();

        virtual void init();

        virtual bool isPsramReady() const;
        virtual bool isSdCardReady() const;
        virtual bool isReady() const;

        /** Takes the pointer to a null terminated character array and prints it to the appropriate destination based on the mode */
        void recordFlightData(char *data);

        void recordLogData(LogType type, const char *data, Dest dest = BOTH);

        void recordLogData(double timeStamp, LogType type, const char *data, Dest dest = BOTH);

        void setRecordMode(Mode mode);

        void dumpData();
    };
} // namespace mmfs

extern mmfs::Logger logger;

#endif // LOGGER_H