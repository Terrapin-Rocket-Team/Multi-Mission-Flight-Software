#ifndef TESTING_LOGGER_H
#define TESTING_LOGGER_H

#include "../../src/RecordData/Logging/Logger.h"

using namespace mmfs;

class TestingLogger : public Logger
{
public:
    TestingLogger()
    {
#ifdef PIO_UNIT_TESTING
        setLogger(this);
#endif
    };
    virtual ~TestingLogger() {};

    SdFs *getSdFs() { return &sd; }
    FsFile *getLogFile() { return &logFile; }
    FsFile *getFlightDataFile() { return &flightDataFile; }
    FsFile *getPreFlightFile() { return &preFlightFile; }
};

#endif // TESTING_LOGGER_H