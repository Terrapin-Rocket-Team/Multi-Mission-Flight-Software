#include "Logger.h"

using namespace mmfs;

static const char *logTypeStrings[] = {"LOG", "ERROR", "WARNING", "INFO"};
static Mode mode = GROUND;

Logger::Logger(GroundDest groundDest = ALTERNATING_BOTH, uint16_t bufferSize = 25000, int bufferInterval = 300) 
{
    ram = new PSRAM();
    dataFile = new SdCardFile();
    logFile = new SdCardFile();
    ready = false;
    dumped = false;

    if (ram->init() && dataFile->init("Log.txt") && logFile->init("FlightData.csv"))
    {
        ready = true;
    }

    // initialize the buffer stuff
    this->groundDest = groundDest;
    this->bufferSize = bufferSize;
    this->interval = bufferInterval;
    bufIdx = 0;
    bufCount = 0;
}


void Logger::recordFlightData(char *data)
{
    // don't bother doing PSRAM if dataFile don't work
    if (!(dataFile->isReady()))
    {
        return;
    }

    if (mode == GROUND)
    {

        // determine if we printing to buffer or what
        if ((groundDest == BUFFER || groundDest == ALTERNATING_BOTH) && ram->isReady())
        {
            // the buffer is the first buffSize bytes of the PSRAM, use it as a circular buffer to store data
            if (bufIdx + strlen(data) + 1 >= bufferSize)
            {
                bufIdx = 0;
                ram->seek(0);       // only seek if we are going to overwrite the buffer (complete messages only)
            }
            ram->println(data);
            bufIdx += strlen(data) + 1;
            bufCount++;

            if (groundDest == ALTERNATING_BOTH && bufCount >= interval)
            {
                dataFile->println(data);
                bufCount = 0;
            }
        }
        else
        {
            dataFile->println(data);
        }
    }
    else if (ram->isReady())
    {
        ram->println(data);
    }
    else if (dataFile->isReady())
    {
        dataFile->println(data);
    }
}

void Logger::recordLogData(LogType type, const char *data, Dest dest)
{
    recordLogData(millis() / 1000.0, type, data, dest);
}

void Logger::recordLogData(double timeStamp, LogType type, const char *data, Dest dest)
{
    int size = 15 + 7; // 15 for the timestamp and extra chars, 7 for the log type
    char logPrefix[size];
    snprintf(logPrefix, size, "%.3f - [%s] ", timeStamp, logTypeStrings[type]);

    if (dest == BOTH || dest == TO_USB)
    {
        Serial.print(logPrefix);
        Serial.println(data);
    }
    if ((dest == BOTH || dest == TO_FILE) && logFile->isReady())
    {
        if (mode == GROUND)
        {
            logFile->print(logPrefix);
            logFile->println(data);
        }
        else if (ram->isReady())
        {
            ram->print(logPrefix, false);
            ram->println(data, false);
        }
        else if (logFile->isReady())
        {
            logFile->print(logPrefix);
            logFile->println(data);
        }
    }
}

void Logger::setRecordMode(Mode m) {
    if (mode == FLIGHT && m == GROUND) {
            
        // Dump the PSRAM to the SD card
        dumpData();

        if (ram->isReady())
            ram->init();

        // Close the log and data files and open new ones
        if (dataFile->isReady())
            dataFile->init("FlightData.csv");
        if (logFile->isReady())
            logFile->init("Log.txt");
    }
    mode = m;
}

void Logger::dumpData() {

    char buffer[512];
    int bytesRead = 0;
    if (ram->isReady() && dataFile->isReady()) {
        // Dump PSRAM to SD card by reading from the PSRAM and writing to the SD card in 512 byte chunks (SD card block size)
        while ((bytesRead = ram->read(buffer, 512)) > 0) {
            dataFile->write(buffer, bytesRead);
        }
    }

    if (ram->isReady() && logFile->isReady()) {
        // do the same but from the bottom of the PSRAM for the log file
        ram->seekFromBottom(0);
        while ((bytesRead = ram->readFromBottom(buffer, 512)) > 0) {
            logFile->write(buffer, bytesRead);
        }

        dumped = true;
    }
}