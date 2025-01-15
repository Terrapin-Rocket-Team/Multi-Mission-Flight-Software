#ifndef TESTING_LOGGER_H
#define TESTING_LOGGER_H

#include "../../src/RecordData/Logger.h"

using namespace mmfs;

class TestingLogger : public Logger
{
    public:
        TestingLogger() {};
        virtual ~TestingLogger() {};

        SdFs &getSdFs() { return sd; }
        FsFile &getLogFile() { return logFile; }
        FsFile &getFlightDataFile() { return flightDataFile; }
        FsFile &getPreFlightFile() { return preFlightFile;}

        PSRAMFile *getRamLogFile() { return ramLogFile; }
        PSRAMFile *getRamFlightDataFile() { return ramFlightDataFile; }
        PSRAMFile *getRamBufferFile() { return ramBufferFile; }

        PSRAM *psram;
};

#endif // TESTING_LOGGER_H