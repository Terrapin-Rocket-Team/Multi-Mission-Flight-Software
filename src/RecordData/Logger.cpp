#include "Logger.h"
#include "../State/State.h"
#include "PSRAMFile.h"

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
    return false;
}

// Returns whether the SD card is ready
bool Logger::isSdCardReady() const
{
    return false;
}

// Returns whether the logger is ready
bool Logger::isReady() const
{
    return false;
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
        ramLogFile = psram->open("Log", true);
        ramFlightDataFile = psram->open("FlightData", true);
        ramBufferFile = psram->open("Buffer", true);
        if (ramLogFile && ramFlightDataFile && ramBufferFile)
            psramReady = true;
    }
    this->state = state;

    recordLogData(INFO_, "This is where the version # would go");
    return sdReady;
}

// Records flight data to the SD card or PSRAM
void Logger::recordFlightData()
{
    if (!sdReady)
    {
        return;
    }
    if (mode == GROUND)
    {
        if (groundMode == SD_)
        {
            //flightDataFile = sd.open(flightDataFileName, O_APPEND);
            //flightDataFile.print(state->getDataString());
            //flightDataFile.println();
            flightDataFile.close();
        }
        else
        {
            if (packData) // can't circular buffer dynamic strings, only fixed length packed data. TODO?
            {
                ramBufferFile->write(state->getPackedData(), state->getPackedDataSize());
                bufferIterations++;
                if (bufferIterations % numBufferLines == 0)
                    ramBufferFile->restart();
            }
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
            //logFile = sd.open(logFileName, O_APPEND);
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

    ramLogFile->restart();
    //logFile = sd.open(logFileName, O_APPEND);
    int size = 1;
    while (size > 0)
    {
        char *data = psram->readNextFileCluster(*ramLogFile, size);
        logFile.write(data, size);
    }
    logFile.close();

    //flightDataFile = sd.open(flightDataFileName, O_APPEND);

    if (packData)
    {
        if (groundMode != SD_) // if set to sd, there is no buffer here.
        {
            // for the buffer, some hacky workaround is needed because of how much data is being dealt with and because it's circular
            char *buffer = new char[150];
            int read;
            int totalRead = 0;
            while ((read = ramBufferFile->read(buffer, state->getPackedDataSize())) > 0)
            { // start by reading froom curPos until end of file, 1kb at a time
                unpackData(buffer);
                totalRead += read;
                flightDataFile.write(buffer, read);
            }
            ramBufferFile->restart();
            int remaining = ramBufferFile->getSize() - totalRead;
            while ((read = ramBufferFile->read(buffer, std::max(1024, remaining))) > 0)
            { // read the remaining data from the beginning of the file
                remaining -= read;
                flightDataFile.write(buffer, read);
            }
        }
    }
    else
    {
        // Now, the real flight data can be read chunk by chunk
        ramFlightDataFile->restart();
        size = 1;
        while (size > 0)
        {
            char *data = psram->readNextFileCluster(*ramFlightDataFile, size);
            flightDataFile.write(data, size);
        }
    }
    flightDataFile.close();
}

void Logger::unpackData(char *dest)
{

}
