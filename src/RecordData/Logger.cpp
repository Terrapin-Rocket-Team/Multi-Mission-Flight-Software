#include "Logger.h"
#include "../State/State.h"
#include "PSRAMFile.h"
#include "DataFormatter.h"

using namespace mmfs;

static const char *logTypeStrings[] = {"LOG", "ERROR", "WARNING", "INFO"};

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
#ifndef PIO_UNIT_TESTING // This is a workaround because testing this logger is hard when it's writing its own variable data to the log file
    char data[100];
    snprintf(data, 100, "This flight is running MMFS v%s", APP_VERSION);
    recordLogData(INFO_, data);
#endif
}

// Destructor for Logger class
Logger::~Logger()
{
    delete[] flightDataFileName;
    delete[] preFlightFileName;
    delete[] logFileName;
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

// Records log data with a timestamp and type
void Logger::recordLogData(LogType type, const char *data, Dest dest)
{
    recordLogData(millis() / 1000.0, type, data, dest);
}

// Records log data with a specific timestamp, type, and destination
void Logger::recordLogData(double timeStamp, LogType type, const char *data, Dest dest) // TODO: Allow custom formatting
{
    int size = 15 + 7; // 15 for the timestamp and extra chars, 7 for the log type
    char logPrefix[size];
    snprintf(logPrefix, size, "%.3f - [%s] ", timeStamp, logTypeStrings[type]);

    if (dest == BOTH || dest == TO_USB)
    {
        Serial.print(logPrefix);
        Serial.println(data);
    }
    if ((dest == BOTH || dest == TO_FILE) && sdReady)
    {
        if (mode != GROUND && psramReady)
        {
            ramLogFile->print(logPrefix);
            ramLogFile->println(data);
        }
        else
        {
            logFile = sd.open(logFileName, FILE_WRITE);
            logFile.print(logPrefix);
            logFile.println(data);
            logFile.close();
        }
    }
}

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
}

Logger &mmfs::getLogger()
{
    static Logger logger;
    return logger;
}