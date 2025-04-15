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

bool LoggingBackendLittleFS::open(const char *filename)
{
    int i = 0;
    while (activeFiles[i] && i < MAX_FILES)
    {
        if (!strcmp(activeFiles[i++]->name(), filename))
            return true;
    }
    if (i < MAX_FILES)
    {
        File *f = new File(lfs->open(filename, FILE_READ | FILE_WRITE));
        if (f)
        {
            activeFiles[i] = f;
            return true;
        }
    }
    return false;
}

size_t LoggingBackendLittleFS::write(int file, const uint8_t *data, size_t len)
{
    if (!activeFiles[file])
        return 0;
    return activeFiles[file]->write(data, len);
}

size_t LoggingBackendLittleFS::write(const char *filename, const uint8_t *data, size_t len)
{
    for (int i = 0; i < MAX_FILES; i++)
    {
        if (!strcmp(activeFiles[i]->name(), filename))
            return write(i, data, len);
    }
    return 0;
}

void LoggingBackendLittleFS::close()
{
}

bool LoggingBackendLittleFS::isAvailable()
{
    return rdy;
}

bool LoggingBackendLittleFS::exists(const char *filename)
{
    return lfs->exists(filename);
}

void LoggingBackendLittleFS::save(int file)
{
    if (activeFiles[file])
    {
        activeFiles[file]->flush();
    }
}

void LoggingBackendLittleFS::save(const char *filename)
{
    for (int i = 0; i < MAX_FILES; i++)
    {
        if (!strcmp(activeFiles[i]->name(), filename))
        {
            activeFiles[i]->flush();
            return;
        }
    }
}