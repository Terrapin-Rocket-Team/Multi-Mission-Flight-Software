#include "Logger.h"
#include "../State/State.h"
#include "PSRAMFile.h"
#include "DataFormatter.h"
#include "../Events/DefaultEvents.h"
#include "../Sensors/GPS/GPS.h"

using namespace mmfs;

static const char *logTypeStrings[] = {"LOG", "ERROR", "WARNING", "INFO", "CUSTOM"};

#pragma region Constructor and Initialization
// Constructor for Logger class
Logger::Logger()
{
    psram = new PSRAM;
    Serial.begin(115200);
    if (sd.begin(SD_CONFIG) || sd.restart())
    {
        sdReady = true;

        // find a unique file name
        char fileName[MAX_FILE_NAME_SIZE];
        int fileNo = 0;
        bool exists = true;
        while (exists)
        {
            snprintf(fileName, MAX_FILE_NAME_SIZE, "%d_%s", ++fileNo, "FlightData.csv");
            exists = sd.exists(fileName);
        }

        // create files

        int len = 26; // max file name length
        flightDataFile = sd.open(fileName, FILE_WRITE);
        flightDataFileName = new char[len];
        snprintf(flightDataFileName, len, "%s", fileName);
        flightDataFile.close();

        snprintf(fileName, MAX_FILE_NAME_SIZE, "%d_%s", fileNo, "Log.txt");
        logFile = sd.open(fileName, FILE_WRITE);
        logFileName = new char[len];
        snprintf(logFileName, len, "%s", fileName);
        logFile.close();

        snprintf(fileName, MAX_FILE_NAME_SIZE, "%d_%s", fileNo, "PreFlightData.csv");
        preFlightFile = sd.open(fileName, FILE_WRITE);
        preFlightFileName = new char[len];
        snprintf(preFlightFileName, len, "%s", fileName);
        preFlightFile.close();
    }
    if (psram->init())
    {
        ramLogFile = psram->open("Log", F_WRITE | F_READ, true);
        ramFlightDataFile = psram->open("FlightData", F_WRITE | F_READ, true);
        ramBufferFile = psram->open("Buffer", F_WRITE | F_READ, true);
        if (ramLogFile && ramFlightDataFile && ramBufferFile)
            psramReady = true;
    }
    setLogPrefixFormatting("$time - [$logType] ");
    setCustomLogPrefix("$time - [CUSTOM] ");
    recordCrashReport();

#ifndef PIO_UNIT_TESTING // This is a workaround because testing this logger is hard when it's writing its own variable data to the log file
    recordLogData(INFO_, 100, "This flight is running MMFS v%s", APP_VERSION);
#endif
}

// Destructor for Logger class
Logger::~Logger()
{
    delete[] flightDataFileName;
    delete[] preFlightFileName;
    delete[] logFileName;
    delete[] logPrefixFormat;
    delete[] customLogPrefix;
    delete ramLogFile;
    delete ramFlightDataFile;
    delete ramBufferFile;
    delete psram;
}

// Returns whether the PSRAM is ready
bool Logger::isPsramReady() const
{
    return psramReady;
}

// Returns whether the SD card is ready
bool Logger::isSdCardReady()
{
    return sdReady || sd.restart();
}

// Returns whether the logger is ready
bool Logger::isReady() const
{
    return ready;
}

// Initializes the logger, returning whether SD card is ready
bool Logger::init(DataReporter **dataReporters, int numReporters, uint16_t bufferTime, int bufferInterval)
{
    this->dataReporters = dataReporters;
    this->numReporters = numReporters;
    this->packData = bufferTime > 0;
    this->bufferTime = bufferTime;
    this->bufferInterval = bufferInterval;
    this->groundMode = bufferInterval > 0 ? ALTERNATE_ : bufferInterval == 0 ? SD_
                                                                             : PSRAM_;
    numBufferLines = bufferTime * UPDATE_RATE;

    return ready = sdReady;
}

#pragma endregion Constructor and Initialization

#pragma region Flight Data Logging
// Records flight data to the SD card or PSRAM
void Logger::recordFlightData()
{
    if (!sdReady && !sd.restart()) // If SD card not available, nothing to do.
    {
        return;
    }
    sdReady = true;

    if (mode == GROUND) // If rocket not in flight
    {
        if (groundMode == SD_) // If we are writing to SD card
        {
            char dest[500];
            DataFormatter::toCSVRow(dest, 500, dataReporters, numReporters);
            flightDataFile = sd.open(preFlightFileName, FILE_WRITE);
            flightDataFile.println(dest);
            flightDataFile.close();
        }
        else
        {
            if (packData) // can't circular buffer dynamic strings, only fixed length packed data. TODO?
            {
                int len = DataFormatter::getPackedLen(dataReporters, numReporters);
                uint8_t *dest = new uint8_t[len];
                DataFormatter::packData(dest, dataReporters, numReporters);
                ramBufferFile->write(dest, len);
                delete[] dest;
                if (++bufferIterations % numBufferLines == 0)
                {
                    ramBufferFile->restart();
                    hasFilledBuffer = true;
                }

                if (groundMode == ALTERNATE_ && bufferIterations % (bufferInterval * UPDATE_RATE) == 0)
                {
                    char dest[500];
                    DataFormatter::toCSVRow(dest, 500, dataReporters, numReporters);
                    flightDataFile = sd.open(preFlightFileName, FILE_WRITE);
                    flightDataFile.println(dest);
                    flightDataFile.close();
                }
            }
        }
    }
    else // If rocket is in flight
    {
        if (psramReady)
        {
            if (!packData) // if not packing data, just write the string to the file
            {
                char dest[500];
                DataFormatter::toCSVRow(dest, 500, dataReporters, numReporters);
                ramFlightDataFile->println(dest);
            }
            else
            {
                int len = DataFormatter::getPackedLen(dataReporters, numReporters);
                uint8_t *dest = new uint8_t[len];
                DataFormatter::packData(dest, dataReporters, numReporters);
                ramFlightDataFile->write(dest, len);
                delete[] dest;
            }
        }
        else // If the PSRAM isn't working, write to the SD card
        {
            char dest[500];
            DataFormatter::toCSVRow(dest, 500, dataReporters, numReporters);
            flightDataFile = sd.open(flightDataFileName, FILE_WRITE);
            flightDataFile.println(dest);
            flightDataFile.close();
        }
    }
}

#pragma endregion Flight Data Logging

#pragma region Log Data Logging

void Logger::recordLogData(const char *msg, Dest dest, LogType type)
{
    int prefixLen = 0;
    if(type == CUSTOM_)
    {
        prefixLen = customLogPrefixLen;
    }
    else if(type != NONE_)
    {
        prefixLen = logPrefixLen;
    }

    if (dest == BOTH || dest == TO_USB)
    {
        Serial.println(msg);
    }
    if ((dest == BOTH || dest == TO_FILE) && sdReady)
    {
        if (mode != GROUND && psramReady)
        {
            if (const char *i = strstr(msg, "\n")) // find the first newline
            {
                int cursor = 0;
                int lenToWrite = i - msg + 1; // length of the string to write
                ramLogFile->write(msg + cursor, lenToWrite);
                cursor += lenToWrite;
                for (i = msg; (i = strstr(i, "\n")) != nullptr; i++) // loop through the string until we find a newline
                {
                    for (int j = 0; j < prefixLen; j++)
                    {
                        ramLogFile->write(" ", 1);
                    }
                    int lenToWrite = i - msg + 1; // length of the string to write
                    ramLogFile->write(msg + cursor, lenToWrite);
                    cursor += lenToWrite;
                }
            }
            else
                ramLogFile->println(msg);
        }
        else
        {
            logFile = sd.open(logFileName, FILE_WRITE);
            if (const char *i = strstr(msg, "\n")) // find the first newline
            {
                int cursor = 0;
                int lenToWrite = i - msg + 1; // length of the string to write
                logFile.write(msg + cursor, lenToWrite);
                cursor += lenToWrite;
                for (i = msg; (i = strstr(i, "\n")) != nullptr; i++) // loop through the string until we find a newline
                {
                    for (int j = 0; j < prefixLen; j++)
                    {
                        logFile.write(" ", 1);
                    }
                    int lenToWrite = i - msg + 1; // length of the string to write
                    logFile.write(msg + cursor, lenToWrite);
                    cursor += lenToWrite;
                }
            }
            else
            {
                logFile.println(msg);
            }
            logFile.close();
        }
    }
    getEventManager().invoke(LogData{"LOG_DATA"_i, dest, type, msg});
}
void Logger::recordLogData(double timeStamp, LogType type, Dest dest, int size, const char *format, va_list args)
{
    int len;
    char *logPrefix = nullptr;
    if (type == CUSTOM_)
    {
        len = customLogPrefixLen;
        logPrefix = new char[customLogPrefixLen];
        snprintf(logPrefix, customLogPrefixLen, customLogPrefix, timeStamp);
    }
    else
    {
        len = logPrefixLen;
        logPrefix = new char[logPrefixLen];
        if (timeFirst)
            snprintf(logPrefix, logPrefixLen, logPrefixFormat, timeStamp, logTypeStrings[type]);
        else
            snprintf(logPrefix, logPrefixLen, logPrefixFormat, logTypeStrings[type], timeStamp);
    }
    char *msg = new char[size + 1];
    int p = vsnprintf(msg, size + 1, format, args);

    char *logMsg = new char[len + p + 1];
    snprintf(logMsg, len + p + 1, "%s%s", logPrefix, msg);
    recordLogData(logMsg, dest, type);
    delete[] msg;
    delete[] logMsg;
    delete[] logPrefix;
}
void Logger::recordLogData(double timeStamp, LogType type, Dest dest, int size, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    recordLogData(timeStamp, type, dest, size, format, args);
    va_end(args);
}
void Logger::recordLogData(LogType type, Dest dest, int size, const char *format, ...)
{

    va_list args;
    va_start(args, format);
    recordLogData(millis(), type, dest, size, format);
    va_end(args);
}
void Logger::recordLogData(LogType type, int size, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    recordLogData(millis(), type, BOTH, size, format, args);
    va_end(args);
}
void Logger::recordLogData(int size, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    char *msg = new char[size + 1];
    vsnprintf(msg, size + 1, format, args);
    va_end(args);
    recordLogData(msg);
    delete[] msg;
}
void Logger::recordLogData(LogType type, Dest dest, const char *msg)
{
    recordLogData(millis(), type, dest, strlen(msg), msg);
}
void Logger::recordLogData(double timeStamp, LogType type, Dest dest, const char *msg)
{
    recordLogData(timeStamp, type, dest, strlen(msg), msg);
}
void Logger::recordLogData(LogType type, const char *msg)
{
    recordLogData(millis(), type, BOTH, strlen(msg), msg);
}
void Logger::recordLogData(double timeStamp, LogType type, const char *msg)
{
    recordLogData(timeStamp, type, BOTH, strlen(msg), msg);
}

void Logger::recordCrashReport()
{
    if(CrashReport)
    {
        recordLogData(ERROR_, 500, "CRASH REPORT: %s", CrashReport);
    }
}

#pragma region Custom Prefixes

void Logger::setCustomLogPrefix(const char *prefix)
{
    unsigned int idxTime = -1;
    if (const char *t = strstr(prefix, "$time"))
        idxTime = t - prefix;

    delete[] customLogPrefix;
    if (idxTime == (unsigned int)-1)
    {
        recordLogData(WARNING_, "Set a custom prefix without $time!");
        customLogPrefixLen = strlen(prefix) + 1;
        customLogPrefix = new char[customLogPrefixLen];
        strcpy(customLogPrefix, prefix);
    }
    else
    {
        customLogPrefixLen = strlen(prefix) + 15 + 1; // 15 for the timestamp and extra chars
        customLogPrefix = new char[customLogPrefixLen];
        unsigned int cursor = 0;
        unsigned int cursorFormat = 0;
        while (cursor < idxTime)
            customLogPrefix[cursorFormat++] = prefix[cursor++];
        strcpy(customLogPrefix + cursorFormat, "%0.3f");
        cursorFormat += 5;
        cursor += sizeof("$time") - 1;
        while (cursor < strlen(prefix))
            customLogPrefix[cursorFormat++] = prefix[cursor++];
        customLogPrefix[cursorFormat] = '\0';
    }
}

void Logger::setCustomLogPrefix(int size, const char *format, ...)
{
    char *msg = new char[size + 1];
    va_list args;
    va_start(args, format);
    vsnprintf(msg, size + 1, format, args);
    va_end(args);
    setCustomLogPrefix(msg);
    delete[] msg;
}

void Logger::setLogPrefixFormatting(const char *prefix)
{
    if (ready)
    {
        recordLogData(ERROR_, "Attempted to set log prefix formatting after Logger already initalized!");
        return;
    }
    unsigned int idxTime = -1;
    unsigned int idxLogType = -1;
    if (const char *t = strstr(prefix, "$time"))
        idxTime = t - prefix;
    if (const char *t = strstr(prefix, "$logType"))
        idxLogType = t - prefix;
    if (idxTime == (unsigned int)-1 || idxLogType == (unsigned int)-1)
    {
        recordLogData(ERROR_, "Attempted to set a log prefix format without $time or $logType!");
        return;
    }

    logPrefixLen = strlen(prefix) + 15 + 1; // 15 for the timestamp and extra chars, 7 for the log type
    delete[] logPrefixFormat;
    logPrefixFormat = new char[logPrefixLen];
    if (idxTime > idxLogType)
    {
        timeFirst = false;
        unsigned int cursorArg = 0;
        unsigned int cursorFormat = 0;

        while (cursorArg < idxLogType)
            logPrefixFormat[cursorFormat++] = prefix[cursorArg++];
        strcpy(logPrefixFormat + cursorFormat, "%s");
        cursorFormat += 2;
        cursorArg += sizeof("$logType") - 1;

        while (cursorArg < idxTime)
            logPrefixFormat[cursorFormat++] = prefix[cursorArg++];
        strcpy(logPrefixFormat + cursorFormat, "%0.3f");
        cursorFormat += 5;
        cursorArg += sizeof("$time") - 1;
        while (cursorArg < strlen(prefix))
            logPrefixFormat[cursorFormat++] = prefix[cursorArg++];
        logPrefixFormat[cursorFormat] = '\0';
    }
    else
    {
        timeFirst = true;
        unsigned int cursorArg = 0;
        unsigned int cursorFormat = 0;

        while (cursorArg < idxTime)
            logPrefixFormat[cursorFormat++] = prefix[cursorArg++];
        strcpy(logPrefixFormat + cursorFormat, "%0.3f");
        cursorFormat += 5;
        cursorArg += sizeof("$time") - 1;

        while (cursorArg < idxLogType)
            logPrefixFormat[cursorFormat++] = prefix[cursorArg++];
        strcpy(logPrefixFormat + cursorFormat, "%s");
        cursorFormat += 2;
        cursorArg += sizeof("$logType") - 1;
        while (cursorArg < strlen(prefix))
            logPrefixFormat[cursorFormat++] = prefix[cursorArg++];
        logPrefixFormat[cursorFormat] = '\0';
    }
}

#pragma endregion Custom Prefixes

#pragma endregion Log Data Logging

#pragma region Event Handling

void Logger::modifyFileDates(const GPS *gps)
{
    flightDataFile.timestamp(T_CREATE | T_WRITE | T_ACCESS, gps->getYear(), gps->getMonth(), gps->getDay(), gps->getHour(), gps->getMinute(), gps->getSecond());
    preFlightFile.timestamp(T_CREATE | T_WRITE | T_ACCESS, gps->getYear(), gps->getMonth(), gps->getDay(), gps->getHour(), gps->getMinute(), gps->getSecond());
    logFile.timestamp(T_WRITE | T_WRITE | T_ACCESS, gps->getYear(), gps->getMonth(), gps->getDay(), gps->getHour(), gps->getMinute(), gps->getSecond());
}

#pragma endregion Event Handling

#pragma region Others
// Sets the recording mode and handles necessary transitions
void Logger::setRecordMode(Mode m)
{
    if (mode == FLIGHT && m == GROUND)
    {
        // Dump the PSRAM to the SD card
        dumpData();
    }
    mode = m;
}

// Dumps data from PSRAM to the SD card
void Logger::dumpData()
{

    if (!psramReady || !sdReady)
    {
        return; // Can't dump data if the PSRAM or SD card isn't working
    }

    // LOG FILE
    ramLogFile->restart();
    logFile = sd.open(logFileName, FILE_WRITE);
    int size = 1;
    while (size > 0)
    {
        char *data = psram->readNextFileCluster(*ramLogFile, size);
        logFile.write(data, size);
    }
    logFile.close();
    // LOG FILE

    // FLIGHT DATA FILE
    flightDataFile = sd.open(flightDataFileName, FILE_WRITE);

    if (packData) // buffer data is packed
    {
        if (groundMode != SD_) // if set to sd or not packing data, there is no buffer here.
        {
            int len = DataFormatter::getPackedLen(dataReporters, numReporters);
            char packed[len];
            char unpacked[500];
            if (hasFilledBuffer) // if the buffer has been filled, then the buffer is circular and we need to read it in chunks
                for (int i = 0; i < numBufferLines; i++)
                {
                    ramBufferFile->read(packed, len);
                    DataFormatter::toCSVRow(unpacked, 500, dataReporters, numReporters, packed);
                    flightDataFile.println(unpacked);
                    if (++bufferIterations % numBufferLines == 0)
                        ramBufferFile->restart();
                }
            else // if the buffer hasn't been filled, then we can just read it all at once
            {
                ramBufferFile->restart();
                while (ramBufferFile->read(packed, len) > 0)
                {
                    DataFormatter::toCSVRow(unpacked, 500, dataReporters, numReporters, packed);
                    flightDataFile.println(unpacked);
                }
            }
        }
    }
    // Now, the real flight data can be read chunk by chunk
    ramFlightDataFile->restart();

    if (!packData) // if the data wasn't packed, then just directly read it from memory to the SD card. It's already in CSV format.
    {
        size = 1;
        while (size > 0)
        {
            char *data = psram->readNextFileCluster(*ramFlightDataFile, size);
            flightDataFile.write(data, size);
        }
    }
    else // unpack the data
    {
        int len = DataFormatter::getPackedLen(dataReporters, numReporters);
        char packed[len];
        char unpacked[500];
        while (ramFlightDataFile->read(packed, len) > 0)
        {
            DataFormatter::toCSVRow(unpacked, 500, dataReporters, numReporters, packed);
            flightDataFile.println(unpacked);
        }
    }
    flightDataFile.close();
}

void Logger::writeCsvHeader()
{
    char header[2000]; // 2000 is arbitrary, but should be enough for basically any header
    DataFormatter::getCSVHeader(header, sizeof(header), dataReporters, numReporters);
    flightDataFile = sd.open(flightDataFileName, FILE_WRITE);
    flightDataFile.println(header);
    flightDataFile.close();
    preFlightFile = sd.open(preFlightFileName, FILE_WRITE);
    preFlightFile.println(header);
    preFlightFile.close();
}
#ifdef PIO_UNIT_TESTING
static Logger *testLogger = nullptr;
void mmfs::setLogger(Logger *logger)
{
    testLogger = logger;
}
#endif
Logger &mmfs::getLogger()
{
#ifdef PIO_UNIT_TESTING
    if (testLogger)
        return *testLogger;
#endif
    static Logger logger;
    return logger;
}