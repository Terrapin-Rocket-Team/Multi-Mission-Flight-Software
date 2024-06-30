#ifndef LOGGER_H
#define LOGGER_H

#include "Psram.h"
#include "SdCardFile.h"

enum LogType
{
    LOG,
    ERROR,
    WARNING,
    INFO
};
enum Dest
{
    BOTH,
    TO_USB,
    TO_FILE
};
enum Mode
{
    FLIGHT,
    GROUND
};

class Logger
{
private:
    PSRAM *ram;
    SdCardFile *dataFile;
    SdCardFile *logFile;
    bool ready;
    bool dumped;

public:

    Logger();
    void recordFlightData(char *data); //0 is preflight, 5 is postflight.
    void recordLogData(LogType type, const char *data, Dest dest = BOTH);
    void recordLogData(double timeStamp, LogType type, const char *data, Dest dest = BOTH);
    void setRecordMode(Mode mode);//Will enable or disable the PSRAM based on the mode. If mode is GROUND, PSRAM will be disabled and all data in PSRAM will be written to the SD card. If mode is FLIGHT, PSRAM will be enabled.

};


#endif //LOGGER_H