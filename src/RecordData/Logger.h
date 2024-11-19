/**
 * @file Logger.h
 * @brief This file contains the declaration of the Logger class, an example use case of the PSRAM and SdCardFile classes.
 * Based on the mode, the Logger class will write data to the PSRAM or the SD card, writing to the SD card when the mode is GROUND
 * and writing to the PSRAM when the mode is FLIGHT. The Logger class will also write data to the USB serial port when connected if
 * the destination is set to TO_USB or BOTH, which it is by default.
 */

#ifndef LOGGER_H
#define LOGGER_H

#define SD_FAT_TYPE 3

#include "psram.h"
#include "SdFat.h"
#include "Arduino.h"
#include "../Constants.h"

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
    class State; // Forward declaration

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

    enum GroundMode
    {
        SD_,
        PSRAM_,
        ALTERNATE_
    };

    class Logger
    {

    public:
        Logger(uint16_t bufferTime = 30, int bufferInterval = 30); // store 30 seconds, print to SD every 30 seconds
        virtual ~Logger();

        virtual bool init(State *state);

        virtual bool isPsramReady() const;
        virtual bool isSdCardReady() const;
        virtual bool isReady() const;

        void recordFlightData(); // records  flight data

        void recordLogData(LogType type, const char *data, Dest dest = BOTH);

        void recordLogData(double timeStamp, LogType type, const char *data, Dest dest = BOTH);

        void setRecordMode(Mode mode);

        void dumpData();

        void writeCsvHeader();

    protected:
        SdFs sd;
        FsFile logFile;
        FsFile flightDataFile;

        //

        Mode mode = GROUND;
        State *state = nullptr;
        GroundMode groundMode = ALTERNATE_;
        bool packData = true;
        uint16_t bufferTime;
        int bufferInterval = 0;
        char *logFileName = nullptr;        // Name of the log file
        char *flightDataFileName = nullptr; // Name of the flight data file
        bool sdReady = false;               // Whether the SD card has been initialized
        bool psramReady = false;            // Whether the PSRAM has been initialized
        bool ready = false;                 // Whether the logger is ready

        //

        PSRAMFile *ramFlightDataFile = nullptr; // Pointer to the flight data file
        PSRAMFile *ramLogFile = nullptr;        // Pointer to the log file
        PSRAMFile *ramBufferFile = nullptr;     // Pointer to the buffer file

        int numBufferLines = 0;
        int bufferIterations = 0;
    };
} // namespace mmfs

extern mmfs::Logger logger;

#endif // LOGGER_H