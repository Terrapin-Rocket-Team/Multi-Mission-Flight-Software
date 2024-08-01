#include "Logger.h"

using namespace mmfs;

static const char *logTypeStrings[] = {"LOG", "ERROR", "WARNING", "INFO"};

// Constructor for Logger class
Logger::Logger(GroundDest groundDest, uint16_t bufferSize, int bufferInterval) {

    this->groundDest = groundDest;
    this->bufferSize = bufferSize;
    this->bufferInterval = bufferInterval;
    bufIdx = 0;
    bufCount = 0;
    mode = GROUND;
    dumped = false;

    if (ram.init() && sdCard.init()) {
        sdCard.addFile("Log.txt");
        sdCard.addFile("FlightData.csv");
        sdCard.selectFile(0);       // since we dk the file name, we select the first file (Log.txt)
        ready = true;
    }
}

// Records flight data to the SD card or PSRAM
void Logger::recordFlightData(char *data) {
    if (!sdCard.isReady()) {
        return;
    }

    if (mode == GROUND) {
        if ((groundDest == BUFFER || groundDest == ALTERNATING_BOTH) && ram.isReady()) {
            // Use PSRAM as a circular buffer
            if (bufIdx + strlen(data) + 1 >= bufferSize) {
                bufIdx = 0;
                ram.seek(0); // Seek to the beginning if we are going to overwrite the buffer
            }
            ram.println(data);
            bufIdx += strlen(data) + 1;
            bufCount++;

            if (groundDest == ALTERNATING_BOTH && bufCount >= bufferInterval) {
                sdCard.selectFile(1);
                sdCard.println(data);
                sdCard.selectFile(0);
                bufCount = 0;
            }
        } else {
            sdCard.selectFile(1);
            sdCard.println(data);
            sdCard.selectFile(0);
        }
    } else if (ram.isReady()) {
        ram.println(data);
    } else if (sdCard.isReady()) {
        sdCard.selectFile(1);
        sdCard.println(data);
        sdCard.selectFile(0);
    }
}

// Records log data with a timestamp and type
void Logger::recordLogData(LogType type, const char *data, Dest dest) {
    recordLogData(millis() / 1000.0, type, data, dest);
}

// Records log data with a specific timestamp, type, and destination
void Logger::recordLogData(double timeStamp, LogType type, const char *data, Dest dest = BOTH) {
    int size = 15 + 7; // 15 for the timestamp and extra chars, 7 for the log type
    char logPrefix[size];
    snprintf(logPrefix, size, "%.3f - [%s] ", timeStamp, logTypeStrings[type]);

    if (dest == BOTH || dest == TO_USB) {
        Serial.print(logPrefix);
        Serial.println(data);
    }
    if ((dest == BOTH || dest == TO_FILE) && sdCard.isReady()) {
        if (mode == GROUND) {
            sdCard.selectFile(0);
            sdCard.print(logPrefix);
            sdCard.println(data);
        } else if (ram.isReady()) {
            ram.print(logPrefix, false);
            ram.println(data, false);
        } else if (sdCard.isReady()) {
            sdCard.selectFile(0);
            sdCard.print(logPrefix);
            sdCard.println(data);
        }
    }
}

// Sets the recording mode and handles necessary transitions
void Logger::setRecordMode(Mode m) {
    if (mode == FLIGHT && m == GROUND) {
        // Dump the PSRAM to the SD card
        dumpData();

        if (ram.isReady())
            ram.init();

        // Reinitialize files on mode change
        if (sdCard.isReady()) {
            sdCard.addFile("FlightData.csv");
            sdCard.selectFile(1);
        }
        if (sdCard.isReady()) {
            sdCard.addFile("Log.txt");
            sdCard.selectFile(0);
        }
    }
    mode = m;
}

// Dumps data from PSRAM to the SD card
void Logger::dumpData() {
    char buffer[512];
    int bytesRead = 0;

    if (ram.isReady() && sdCard.isReady()) {
        // Dump PSRAM to FlightData.csv
        sdCard.selectFile(1);
        while ((bytesRead = ram.read(buffer, 512)) > 0) {
            sdCard.write(buffer, bytesRead);
        }
    }

    if (ram.isReady() && sdCard.isReady()) {
        // Dump log data from the bottom of the PSRAM to Log.txt
        sdCard.selectFile(0);
        ram.seekFromBottom(0);
        while ((bytesRead = ram.readFromBottom(buffer, 512)) > 0) {
            sdCard.write(buffer, bytesRead);
        }
        dumped = true;
    }
}
