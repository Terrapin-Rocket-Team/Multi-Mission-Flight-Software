#include "LoggingBackendSdFat.h"

using namespace mmfs;

LoggingBackendSdFat::LoggingBackendSdFat()
{
    for (unsigned int i = 0; i < MAX_FILES; i++)
        activeFiles[i] = nullptr;
    sdfs = new SdFs();
}
LoggingBackendSdFat::~LoggingBackendSdFat()
{

    for (unsigned int i = 0; i < MAX_FILES; i++)
    {
        if (activeFiles[i])
        {
            activeFiles[i]->close();
        }
    }
    delete sdfs;
}

bool LoggingBackendSdFat::begin()
{
    return rdy = sdfs != nullptr && (sdfs->begin() || sdfs->restart());
}

LoggingBackendFile *LoggingBackendSdFat::open(const char *filename)
{
    unsigned int i = 0;
    while (activeFiles[i] && i < MAX_FILES)
    {
        char fname[250];
        activeFiles[i]->getName(fname, 250);
        if (!strcmp(fname, filename))
            return new LoggingBackendFile(i);
        i++;
    }
    if (i < MAX_FILES)
    {
        FsFile *f = new FsFile(sdfs->open(filename, O_RDWR));
        if (f)
        {
            activeFiles[i] = f;
            return new LoggingBackendFile(i);
        }
    }
    return nullptr;
}

size_t LoggingBackendSdFat::write(int file, const uint8_t *data, size_t len)
{
    if (!activeFiles[file])
        return 0;
    return activeFiles[file]->write(data, len);
}

bool LoggingBackendSdFat::isAvailable()
{
    return rdy;
}

bool LoggingBackendSdFat::exists(const char *filename)
{
    return sdfs->exists(filename);
}