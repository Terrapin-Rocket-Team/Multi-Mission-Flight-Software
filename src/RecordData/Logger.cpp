#include "Logger.h"
#include "../State/State.h"
#include "PSRAMFile.h"
#include "DataFormatter.h"

using namespace mmfs;

static const char *logTypeStrings[] = {"LOG", "ERROR", "WARNING", "INFO"};

// Constructor for Logger class
Logger::Logger(uint16_t bufferTime, int bufferInterval, bool packData, GroundMode groundMode)
{
    this->packData = packData;
    this->bufferTime = bufferTime;
    this->bufferInterval = bufferInterval;
    this->groundMode = groundMode;
    numBufferLines = bufferTime * UPDATE_RATE;
}

// Destructor for Logger class
Logger::~Logger()
{
}

// Returns whether the PSRAM is ready
bool Logger::isPsramReady() const
{
    return psramReady;
}

// Returns whether the SD card is ready
bool Logger::isSdCardReady() const
{
    return sdReady;
}

// Returns whether the logger is ready
bool Logger::isReady() const
{
    return ready;
}

// Initializes the logger, returning whether SD card is ready
bool Logger::init(State *state)
{

    if (sd.begin(SD_CONFIG) || sd.restart())
    {
        sdReady = true;
        char fileName[MAX_FILE_NAME_SIZE];
        int fileNo = 0;
        bool exists = true;
        while (exists)
        {
            snprintf(fileName, MAX_FILE_NAME_SIZE, "%d_%s", ++fileNo, "FlightData.csv");
            exists = sd.exists(fileName);
        }
        flightDataFile = sd.open(fileName, FILE_WRITE);
        flightDataFileName = new char[strlen(fileName) + 1];
        snprintf(flightDataFileName, strlen(fileName) + 1, "%s", fileName);
        flightDataFile.close();

        snprintf(fileName, MAX_FILE_NAME_SIZE, "%d_%s", fileNo, "Log.txt");
        logFile = sd.open(fileName, FILE_WRITE);
        logFileName = new char[strlen(fileName) + 1];
        snprintf(logFileName, strlen(fileName) + 1, "%s", fileName);
        logFile.close();
    }
    if (psram->init())
    {
        ramLogFile = psram->open("Log", F_WRITE, true);
        ramFlightDataFile = psram->open("FlightData", F_WRITE, true);
        ramBufferFile = psram->open("Buffer", F_WRITE, true);
        if (ramLogFile && ramFlightDataFile && ramBufferFile)
            psramReady = true;
    }
    this->state = state;
#ifndef PIO_UNIT_TESTING
    recordLogData(INFO_, "This is where the version # would go");
#endif
    ready = true;
    return sdReady;
}

// Records flight data to the SD card or PSRAM
void Logger::recordFlightData()
{
    if (!sdReady) // If SD card not available, nothing to do.
    {
        return;
    }
    if (mode == GROUND) // If rocket not in flight
    {
        if (groundMode == SD_) // If we are writing to SD card
        {
            char dest[500];
            DataFormatter::toCSVRow(dest, 500, state);
            flightDataFile = sd.open(flightDataFileName, O_APPEND);
            flightDataFile.println(dest);
            flightDataFile.close();
        }
        else
        {
            if (packData) // can't circular buffer dynamic strings, only fixed length packed data. TODO?
            {
                int len = DataFormatter::getPackedLen(state);
                uint8_t *dest = new uint8_t[len];
                DataFormatter::packData(dest, state);
                ramBufferFile->write(dest, len);
                delete[] dest;
                if (++bufferIterations % numBufferLines == 0)
                    ramBufferFile->restart();

                if (groundMode == ALTERNATE_)
                // TODO: This prints the most recent data. We really want it to print the oldest data in the buffer
                // because when the buffer is dumped, it will start at the oldest data. This will result in one of the lines
                // being out of time order. This is a minor issue, but it would be nice to fix it.
                {
                    char dest[500];
                    DataFormatter::toCSVRow(dest, 500, state);
                    flightDataFile = sd.open(flightDataFileName, O_APPEND);
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
                DataFormatter::toCSVRow(dest, 500, state);
                ramFlightDataFile->println(dest);
            }
            else
            {
                int len = DataFormatter::getPackedLen(state);
                uint8_t *dest = new uint8_t[len];
                DataFormatter::packData(dest, state);
                ramFlightDataFile->write(dest, len);
                delete[] dest;
            }
        }
        else // If the PSRAM isn't working, write to the SD card
        {
            char dest[500];
            DataFormatter::toCSVRow(dest, 500, state);
            flightDataFile = sd.open(flightDataFileName, O_APPEND);
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
            logFile = sd.open(logFileName, O_APPEND);
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
    logFile = sd.open(logFileName, O_APPEND);
    int size = 1;
    while (size > 0)
    {
        char *data = psram->readNextFileCluster(*ramLogFile, size);
        logFile.write(data, size);
    }
    logFile.close();
    // LOG FILE

    // FLIGHT DATA FILE
    flightDataFile = sd.open(flightDataFileName, O_APPEND);

    if (packData)
    {
        if (groundMode != SD_) // if set to sd or not packing data, there is no buffer here.
        {
            int len = DataFormatter::getPackedLen(state);
            char packed[len];
            char unpacked[500];
            for (int i = 0; i < numBufferLines; i++)
            {
                ramBufferFile->read(packed, len);
                DataFormatter::toCSVRow(unpacked, 500, state, packed);
                flightDataFile.println(unpacked);
                if (++bufferIterations % numBufferLines == 0)
                    ramBufferFile->restart();
            }
        }
    }
    // Now, the real flight data can be read chunk by chunk
    ramFlightDataFile->restart();
    if(!packData)
    {
        size = 1;
        while (size > 0)
        {
            char *data = psram->readNextFileCluster(*ramFlightDataFile, size);
            flightDataFile.write(data, size);
        }
    }
    else
    {
        int len = DataFormatter::getPackedLen(state);
        char packed[len];
        char unpacked[500];
        while(ramFlightDataFile->read(packed, len) > 0)
        {
            DataFormatter::toCSVRow(unpacked, 500, state, packed);
            flightDataFile.println(unpacked);
        }
    }
    flightDataFile.close();
}
