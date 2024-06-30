#include "Logger.h"

static const char *logTypeStrings[] = {"LOG", "ERROR", "WARNING", "INFO"};
static Mode mode = GROUND;

Logger::Logger()
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
}

void Logger::recordFlightData(char *data)
{
    if (!(dataFile->isReady()))
    {
        return;
    }
    if (mode == GROUND)
    {
        dataFile->println(data);
    }
    else if (ram->isReady())
    {
        ram->println(data);
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
    }
}

void Logger::setRecordMode(Mode m) {
    if (mode == FLIGHT && m == GROUND) {
        if (ram->isReady()) {
            // Dump PSRAM to SD card by reading from the PSRAM and writing to the SD card
            char buffer[512];
            int bytesRead = 0;
            while ((bytesRead = ram->read(buffer, 512)) > 0) {
                dataFile->write(buffer, bytesRead);
            }
        }
    }
    mode = m;
}