#include "LoggingBackendLittleFS.h"
#include <LittleFS.h>

using namespace mmfs;

namespace mmfs::littlefs // declare some helper functions used at the end
{
    void printDirectory(Stream &s, File dir, int numSpaces);
    void printTime(Stream &s, const DateTimeFields tm);
} // namespace littlefs

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
    rdy = lfs != nullptr && lfs->begin();
    return rdy;
}

LoggingBackendFile *LoggingBackendLittleFS::open(const char *filename)
{

    unsigned int i = 0;
    while (activeFiles[i] && i < MAX_FILES) // if exists
    {
        if (!strcmp(activeFiles[i]->name(), filename))
            return new LoggingBackendFile(this, i);
        i++;
    }

    // if doesnt exist
    if (i < MAX_FILES)
    {
        File *f = new File(lfs->open(filename, FILE_READ | FILE_WRITE));
        if (f)
        {
            activeFiles[i] = f;
            return new LoggingBackendFile(this, i);
        }
    }
    return nullptr; // if can't create
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

void LoggingBackendLittleFS::save(int file)
{
    if (activeFiles[file])
    {
        activeFiles[file]->flush();
    }
}

void LoggingBackendLittleFS::ls(Stream &s)
{
    littlefs::printDirectory(s, lfs->open("/"), 0);
    Serial.println();
}

void LoggingBackendLittleFS::format()
{
    lfs->format();
}

bool LoggingBackendLittleFS::remove(const char *filename)
{
    return lfs->remove(filename);
}

size_t LoggingBackendLittleFS::read(int file, char *dest, size_t len)
{
    if (activeFiles[file])
    {
        int i = activeFiles[file]->readBytes(dest, len);
        return i;
    }
    return 0;
}

void LoggingBackendLittleFS::seek(int file, long pos)
{
    if (activeFiles[file])
        activeFiles[file]->seek(pos);
}

// From https://github.com/PaulStoffregen/LittleFS/blob/main/examples/ListFiles/ListFiles.ino
void mmfs::littlefs::printDirectory(Stream &s, File dir, int numSpaces)
{
    while (true)
    {
        File entry = dir.openNextFile();
        if (!entry)
        {
            s.println();
            break;
        }
        s.print(entry.name());
        if (entry.isDirectory())
        {
            s.println('/');
            printDirectory(s, entry, numSpaces + 2);
        }
        else
        {
            // files have sizes, directories do not
            s.print(',');
            s.print(entry.size(), DEC);
            s.print(',');
            DateTimeFields datetime;
            if (entry.getModifyTime(datetime))
            {
                printTime(s, datetime);
            }
            s.println();
        }
        entry.close();
    }
}

// From https://github.com/PaulStoffregen/LittleFS/blob/main/examples/ListFiles/ListFiles.ino
void mmfs::littlefs::printTime(Stream &s, const DateTimeFields tm)
{
    // YYYY-MM-DD hh:mm
    if (tm.year + 1900 < 2025) // Did not get a valid date or time from GPS or RTC
    {
        s.print("N/A");
        return;
    }

    s.print(tm.year + 1900);
    s.print('-');
    if (tm.mon < 10)
        s.print('0');
    s.print(tm.mon < 12 ? tm.mon : '?');
    s.print('-');
    if(tm.mday < 10)
        s.print('0');
    s.print(tm.mday);

    s.print(' ');

    if (tm.hour < 10)
        s.print('0');
    s.print(tm.hour);
    s.print(':');
    if (tm.min < 10)
        s.print('0');
    s.print(tm.min);
}