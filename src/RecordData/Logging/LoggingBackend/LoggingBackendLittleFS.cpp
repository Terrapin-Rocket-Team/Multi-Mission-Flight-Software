#include "LoggingBackendLittleFS.h"

using namespace mmfs;

LoggingBackendLittleFS::LoggingBackendLittleFS()
{
    for (int i = 0; i < MAX_FILES; i++)
        activeFiles[i] = nullptr;
    lfs = new LittleFS_QPINAND();
}
LoggingBackendLittleFS::~LoggingBackendLittleFS()
{

    for (int i = 0; i < MAX_FILES; i++)
    {
        if (activeFiles[i])
        {
            activeFiles[i]->close();
            delete activeFiles[i];
        }
    }
    delete lfs;
}

bool LoggingBackendLittleFS::begin()
{
    return rdy = lfs != nullptr && lfs->begin();
}

LoggingBackendFile *LoggingBackendLittleFS::open(const char *filename)
{
    int i = 0;
    while (activeFiles[i] && i < MAX_FILES)
    {
        if (!strcmp(activeFiles[i++]->name(), filename))
        return new LoggingBackendFile(i);
    }
    if (i < MAX_FILES)
    {
        File *f = new File(lfs->open(filename, FILE_READ | FILE_WRITE));
        if (f)
        {
            activeFiles[i] = f;
            return new LoggingBackendFile(i);
        }
    }
    return nullptr;
}

size_t LoggingBackendLittleFS::write(int file, const uint8_t *data, size_t len)
{
    if (!activeFiles[file])
        return 0;
    return activeFiles[file]->write(data, len);
}

bool LoggingBackendLittleFS::isAvailable()
{
    return rdy;
}

bool LoggingBackendLittleFS::exists(const char *filename)
{
    return lfs->exists(filename);
}