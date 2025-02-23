/**
 * @file Logger.h
 * @brief This file contains the declaration of the Logger class, an example use case of the PSRAM and SdCardFile classes.
 * Based on the mode, the Logger class will write data to the PSRAM or the SD card, writing to the SD card when the mode is GROUND
 * and writing to the PSRAM when the mode is FLIGHT. The Logger class will also write data to the USB serial port when connected if
 * the destination is set to TO_USB or BOTH, which it is by default.
 */

#ifndef LOGGER_H
#define LOGGER_H


#include "../Constants.h"
#include "SdFatBoilerplate.h"
#include <stdarg.h>
#include "../Events/Event.h"
#include <LittleFS.h>

namespace mmfs
{
    class State;        // Forward declaration
    class DataReporter; // Forward declaration
    class GPS;

    enum LogType
    {
        LOG_,
        ERROR_,
        WARNING_,
        INFO_,
        CUSTOM_,
        NONE_
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
        Logger(); // store 30 seconds, print to SD every 30 seconds
        virtual ~Logger();

        virtual bool init(DataReporter **dataReporters, int numReporters);

        virtual bool isSdCardReady();
        virtual bool isFlashReady() const;
        virtual bool isReady() const;

        void recordFlightData(); // records  flight data

        // recordLogData with format string
        void recordLogData(LogType type, Dest dest, int size, const char *format, ...);
        void recordLogData(double timeStamp, LogType type, Dest dest, int size, const char *format, ...);
        void recordLogData(LogType type, int size, const char *format, ...);
        void recordLogData(int size, const char *format, ...);

        // recordLogData with no format string
        void recordLogData(LogType type, Dest dest, const char *msg);
        void recordLogData(double timeStamp, LogType type, Dest dest, const char *msg);
        void recordLogData(LogType type, const char *msg);
        void recordLogData(double timeStamp, LogType type, const char *msg);

        // Custom prefix

        // use $time to wite 3 decimals of time data.
        // ex: setLogPrefix("[$time] - CUSTOM");
        void setCustomLogPrefix(const char *prefix);
        // use $time to wite 3 decimals of time data.
        // ex: setLogPrefix("[$time] - CUSTOM");
        void setCustomLogPrefix(int size, const char *format, ...);

        //use $time and $logType to access the time and log type of the current log.
        // ex: setLogPrefix("[$time] - $logType:"); -> [0.000] - [INFO]: {log message}
        void setLogPrefixFormatting(const char *prefix);

        void setRecordMode(Mode mode);

        void writeCsvHeader();

        void setGroundMode(GroundMode mode)
        {
            if (!ready)
                this->groundMode = mode;
            else
                recordLogData(WARNING_, "Attempted to set GroundMode value after Logger already initalized!");
        }

        bool getPackData() const { return packData; }
        GroundMode getGroundMode() const { return groundMode; }

        void modifyFileDates(const GPS *gps);

    protected:
        void recordLogData(double timeStamp, LogType type, Dest dest, int size, const char *format, va_list args);
        void recordLogData(const char *msg, Dest dest = BOTH, LogType type = NONE_);
        void recordCrashReport();
        SdFs sd;
        FsFile logFile;
        FsFile flightDataFile;
        FsFile preFlightFile;
        LittleFS_QPINAND flash;
        File flightDataFlashFile;

        //

        Mode mode = GROUND;
        DataReporter **dataReporters = nullptr;
        int numReporters = 0;
        GroundMode groundMode = ALTERNATE_;
        bool packData = true;
        uint16_t bufferTime;
        int bufferInterval = 0;
        char *logFileName = nullptr;        // Name of the log file
        char *flightDataFileName = nullptr; // Name of the flight data file
        char *preFlightFileName = nullptr;  // Name of the pre-flight file
        bool sdReady = false;               // Whether the SD card has been initialized
        bool flashReady = false;            // Whether the flash has been initialized
        bool ready = false;                 // Whether the logger is ready

        char *logPrefixFormat = nullptr;
        int logPrefixLen = 0;
        char *customLogPrefix = nullptr;
        int customLogPrefixLen = 0;
        bool timeFirst = true;

    };

    Logger &getLogger();
    #ifdef PIO_UNIT_TESTING
    void setLogger(Logger *logger);
    #endif
} // namespace mmfs

#endif // LOGGER_H