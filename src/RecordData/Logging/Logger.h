/**
 * @file Logger.h
 * @brief This file contains the declaration of the Logger class, an example use case of the PSRAM and SdCardFile classes.
 * Based on the mode, the Logger class will write data to the PSRAM or the SD card, writing to the SD card when the mode is GROUND
 * and writing to the PSRAM when the mode is FLIGHT. The Logger class will also write data to the USB serial port when connected if
 * the destination is set to TO_USB or BOTH, which it is by default.
 */

#ifndef LOGGER_H
#define LOGGER_H


#include "../../Constants.h"
#include "LoggingBackend/LoggingBackend.h"
#include <stdarg.h>
#include "../../Events/Event.h"

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

        virtual bool isReady() const;

        int getFlightNum(); // returns # extension of current flight

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

        void modifyFileDates(const GPS *gps);
        LoggingBackend *backend;
        void recordCrashReport();
        
        LoggingBackendFile *logFile;
        LoggingBackendFile *flightDataFile;
        LoggingBackendFile *preFlightFile;
    protected:
        void recordLogData(double timeStamp, LogType type, Dest dest, int size, const char *format, va_list args);
        void recordLogData(const char *msg, Dest dest = BOTH, LogType type = NONE_);

        //

        Mode mode = GROUND;
        DataReporter **dataReporters = nullptr;
        int numReporters = 0;
        uint16_t bufferTime;
        int bufferInterval = 0;
        char *logFileName = nullptr;        // Name of the log file
        char *flightDataFileName = nullptr; // Name of the flight data file
        char *preFlightFileName = nullptr;  // Name of the pre-flight file
        bool ready = false;                 // Whether the logger is ready

        char *logPrefixFormat = nullptr;
        int logPrefixLen = 0;
        char *customLogPrefix = nullptr;
        int customLogPrefixLen = 0;
        bool timeFirst = true;
        int flightNum = 0;
    };

    Logger &getLogger();
    #ifdef NATIVE
    void setLogger(Logger *logger);
    #endif
} // namespace mmfs

#endif // LOGGER_H