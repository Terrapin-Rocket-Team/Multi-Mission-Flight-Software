#ifndef TESTING_LOGGER_H
#define TESTING_LOGGER_H

#include "../../src/RecordData/Logger.h"

using namespace mmfs;

class TestingLogger : public Logger
{
    public:
        TestingLogger(uint16_t bufferTime = 30, int bufferInterval = 30, bool packData = true, GroundMode mode = ALTERNATE_) : Logger(bufferTime, bufferInterval, packData, mode) {};
        virtual ~TestingLogger() {};

        SdFs &getSdFs() { return sd; };
        FsFile &getLogFile() { return logFile; };
        FsFile &getFlightDataFile() { return flightDataFile; };

        PSRAMFile *getRamLogFile() { return ramLogFile; };
        PSRAMFile *getRamFlightDataFile() { return ramFlightDataFile; };
        PSRAMFile *getRamBufferFile() { return ramBufferFile; };
};

#endif // TESTING_LOGGER_H