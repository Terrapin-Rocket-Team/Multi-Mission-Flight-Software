#include "LoggingBackendSdFat.h"

using namespace mmfs;

LoggingBackendSdFat::LoggingBackendSdFat()
{
    sdfs = new SdFs();
}

LoggingBackendSdFat::~LoggingBackendSdFat()
{

    for (unsigned int i = 0; i < MAX_FILES; i++)
    {
        if (activeFiles[i])
        {
            activeFiles[i].close();
        }
    }
    delete sdfs;
}

bool LoggingBackendSdFat::begin()
{
    return rdy = sdfs != nullptr && (sdfs->begin(SD_CONFIG) || sdfs->restart());
}

LoggingBackendFile *LoggingBackendSdFat::open(const char *filename, uint8_t flags)
{

    int sdfsFlags = 0;
    if (flags == FI_READ)
        sdfsFlags = FILE_READ;
    else if (flags == FI_WRITE_BEGINNING)
        sdfsFlags = O_RDWR | O_CREAT;
    else if (flags == FI_WRITE)
        sdfsFlags = FILE_WRITE;
    else
        return nullptr;

    unsigned int i = 0;
    while (i < MAX_FILES && activeFiles[i]) // if exists
    {
        char fname[250];
        activeFiles[i].getName(fname, 250);
        if (!strcmp(fname, filename))
        {
            activeFiles[i].open(filename, sdfsFlags);
            if (activeFiles[i].isOpen())
                return new LoggingBackendFile(this, i);
            activeFiles[i].close();
        }
        i++;
    }
    // if doesnt exist
    if (i < MAX_FILES)
    {
        activeFiles[i].open(filename, sdfsFlags);
        if (activeFiles[i].isOpen())
            return new LoggingBackendFile(this, i);
        activeFiles[i].close();
    }
    // if can't create
    return nullptr;
}

size_t LoggingBackendSdFat::write(int file, const uint8_t *data, size_t len)
{
    if (!activeFiles[file])
    {
        Serial.println("file not found to print to");
        return 0;
    }
    return activeFiles[file].write(data, len);
}

bool LoggingBackendSdFat::isAvailable()
{
    return rdy;
}

bool LoggingBackendSdFat::exists(const char *filename)
{
    return sdfs->exists(filename);
}

void LoggingBackendSdFat::close(int file)
{
    if (activeFiles[file])
    {
        activeFiles[file].close();
    }
}

void LoggingBackendSdFat::save(int file)
{
    if (activeFiles[file])
    {
        activeFiles[file].flush();
    }
}

void LoggingBackendSdFat::ls(int i)
{
    // TODO...
    Serial.println("Not yet implemented. Come back later :P");
}

void LoggingBackendSdFat::format()
{
    sdfs->format();
}

bool LoggingBackendSdFat::remove(const char *filename)
{
    return sdfs->remove(filename);
}

size_t LoggingBackendSdFat::read(int file, char *dest, size_t len)
{
    if (activeFiles[file])
        return activeFiles[file].readBytes(dest, len);
    return 0;
}

void LoggingBackendSdFat::seek(int file, long pos)
{
    if (activeFiles[file])
        activeFiles[file].seek(pos);
}