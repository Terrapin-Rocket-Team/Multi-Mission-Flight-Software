#include "LoggingBackendLittleFS.h"

using namespace mmfs;

LoggingBackendLittleFS::LoggingBackendLittleFS()
{
    for (unsigned int i = 0; i < MAX_FILES; i++)
        activeFiles[i] = nullptr;
    lfs = new LittleFS_QPINAND();
}
LoggingBackendLittleFS::~LoggingBackendLittleFS()
{

    for (unsigned int i = 0; i < MAX_FILES; i++)
    {
        if (activeFiles[i])
        {
            activeFiles[i]->close();
            delete activeFiles[i];
        }
    }
}

bool LoggingBackendLittleFS::begin()
{
    return rdy = lfs != nullptr && lfs->begin();
}

LoggingBackendFile *LoggingBackendLittleFS::open(const char *filename)
{
    unsigned int i = 0;
    while (i < MAX_FILES)
    {
        if (!strcmp(activeFiles[i++]->name(), filename))
            return new LoggingBackendFile(this, i);
    }
    if (i < MAX_FILES)
    {
        File *f = new File(lfs->open(filename, FILE_READ | FILE_WRITE));
        if (f)
        {
            activeFiles[i] = f;
            return new LoggingBackendFile(this, i);
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

void LoggingBackendLittleFS::close(int file)
{
    if (activeFiles[file])
    {
        activeFiles[file]->close();
        delete activeFiles[file];
        activeFiles[file] = nullptr;
    }
}

void LoggingBackendLittleFS::save(int file){
    if(activeFiles[file]){
        activeFiles[file]->flush();
    }
}

void LoggingBackendLittleFS::ls(int i){
    // TODO...
    Serial.println("Not yet implemented. Come back later :P");
}

void LoggingBackendLittleFS::format(){
    lfs->format();
}

bool LoggingBackendLittleFS::remove(const char *filename){
    return lfs->remove(filename);
}

size_t LoggingBackendLittleFS::read(int file, char *dest, size_t len){
    if(activeFiles[file])
        return activeFiles[file]->readBytes(dest, len);
    return 0;
}