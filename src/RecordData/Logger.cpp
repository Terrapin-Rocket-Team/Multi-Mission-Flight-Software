#include "Logger.h"
#include "../State/State.h"
#include "PSRAMFile.h"

using namespace mmfs;

static const char *logTypeStrings[] = {"LOG", "ERROR", "WARNING", "INFO"};

// Constructor for Logger class
Logger::Logger(uint16_t bufferTime, int bufferInterval, bool packData)
{
    this->packData = packData;
    this->bufferTime = bufferTime;
    this->bufferInterval = bufferInterval;
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
bool Logger::init()
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
        flightDataFile.close();
        snprintf(fileName, MAX_FILE_NAME_SIZE, "%d_%s", fileNo, "Log.txt");
        logFile = sd.open(fileName, FILE_WRITE);
        logFile.close();
    }
    if (psram->init()){
        ramLogFile = psram->open("Log", true);
        ramFlightDataFile = psram->open("FlightData", true);
        ramBufferFile = psram->open("Buffer", true);
        if(ramLogFile && ramFlightDataFile && ramBufferFile){
            psramReady = true;
        }
    }

    recordLogData(INFO_, "This is where the version # would go");
    return sdReady;
}

// Records flight data to the SD card or PSRAM
void Logger::recordFlightData(State &state)
{

}

// Records log data with a timestamp and type
void Logger::recordLogData(LogType type, const char *data, Dest dest)
{
    recordLogData(millis() / 1000.0, type, data, dest);
}

// Records log data with a specific timestamp, type, and destination
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
    if ((dest == BOTH || dest == TO_FILE) && sdReady)
    {
        if (mode == GROUND)
        {
           // flightDataFile.open(flightDataFile.)
        }
        else if (psramReady)
        {
            // ram.print(logPrefix, false);
            // ram.println(data, false);
        }
        else if (sdReady)
        {
            // sdCard.selectFile(0);
            // sdCard.print(logPrefix);
            // sdCard.println(data);
        }
    }
}

// Sets the recording mode and handles necessary transitions
void Logger::setRecordMode(Mode m)
{
    // if (mode == FLIGHT && m == GROUND) {
    //     // Dump the PSRAM to the SD card
    //     dumpData();

    //     if (!ram.isReady())
    //         ram.init();

    //     // Reinitialize files on mode change
    //     if (sdCard.isReady()) {
    //         sdCard.addFile("FlightData.csv");
    //         sdCard.selectFile(1);
    //     }
    //     if (sdCard.isReady()) {
    //         sdCard.addFile("Log.txt");
    //         sdCard.selectFile(0);
    //     }
    // }
    // mode = m;
}

// Dumps data from PSRAM to the SD card
void Logger::dumpData()
{
    // char buffer[512];
    // int bytesRead = 0;

    // if (ram.isReady() && sdCard.isReady()) {
    //     // Dump PSRAM to FlightData.csv
    //     sdCard.selectFile(1);
    //     //while ((bytesRead = ram.read(buffer, 512)) > 0) {
    //         //sdCard.write(buffer, bytesRead);
    //     //}
    // }

    // if (ram.isReady() && sdCard.isReady()) {
    //     // Dump log data from the bottom of the PSRAM to Log.txt
    //     sdCard.selectFile(0);
    //     //ram.seekFromBottom(0);
    //     //while ((bytesRead = ram.readFromBottom(buffer, 512)) > 0) {
    //         //sdCard.write(buffer, bytesRead);
    //     //}
    //     dumped = true;
    // }
}
