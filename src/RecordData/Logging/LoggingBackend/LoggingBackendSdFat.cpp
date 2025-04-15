#include "LoggingBackendSdFat.h"

using namespace mmfs;

LoggingBackendSdFat::LoggingBackendSdFat()
{
    for (int i = 0; i < MAX_FILES; i++)
        activeFiles[i] = nullptr;
    sd = new SdFs();
}
LoggingBackendSdFat::~LoggingBackendSdFat()
{

    for (int i = 0; i < MAX_FILES; i++)
    {
        if (activeFiles[i])
        {
            activeFiles[i]->close();
            delete activeFiles[i];
        }
    }
    delete sd;
}

bool LoggingBackendSdFat::begin()
{
    return rdy = sd != nullptr && (sd->begin() || sd->restart());
}

bool LoggingBackendSdFat::open(const char *filename)
{
    int i = 0;
    while (activeFiles[i] && i < MAX_FILES)
    {
        if (!strcmp(activeFiles[i++]->name(), filename))
            return true;
    }
    if (i < MAX_FILES)
    {
        FsFile *f = new FsFile(sd->open(filename, O_RDWR));
        if (f)
        {
            activeFiles[i] = f;
            return true;
        }
    }
    return false;
}

size_t LoggingBackendSdFat::write(int file, const uint8_t *data, size_t len)
{
    if (!activeFiles[file])
        return 0;
    return activeFiles[file]->write(data, len);
}

size_t LoggingBackendSdFat::write(const char *filename, const uint8_t *data, size_t len)
{
    for (int i = 0; i < MAX_FILES; i++)
    {
        if (!strcmp(activeFiles[i]->name(), filename))
            return write(i, data, len);
    }
    return 0;
}

void LoggingBackendSdFat::close()
{
}

bool LoggingBackendSdFat::isAvailable()
{
    return rdy;
}

bool LoggingBackendSdFat::exists(const char *filename)
{
    return sd->exists(filename);
}

void LoggingBackendSdFat::save(int file)
{
    if (activeFiles[file])
    {
        activeFiles[file]->flush();
    }
}

void LoggingBackendSdFat::save(const char *filename)
{
    for (int i = 0; i < MAX_FILES; i++)
    {
        if (!strcmp(activeFiles[i]->name(), filename)){
            activeFiles[i]->flush();
            return;
        }
    }
}