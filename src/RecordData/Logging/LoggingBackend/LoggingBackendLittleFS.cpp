#include "LoggingBackendLittleFS.h"
#include <LittleFS.h>

using namespace mmfs;

namespace mmfs::littlefs // declare some helper functions used at the end
{
    void printDirectory(File dir, int numSpaces);
    void printSpaces(int num);
    void printTime(const DateTimeFields tm);
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

void LoggingBackendLittleFS::ls(int i)
{
    littlefs::printDirectory(lfs->open("/"), 0);
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
        return activeFiles[file]->readBytes(dest, len);
    return 0;
}

// From https://github.com/PaulStoffregen/LittleFS/blob/main/examples/ListFiles/ListFiles.ino
void littlefs::printDirectory(File dir, int numSpaces)
{
    while (true)
    {
        File entry = dir.openNextFile();
        if (!entry)
        {
            // Serial.println("** no more files **");
            break;
        }
        printSpaces(numSpaces);
        Serial.print(entry.name());
        if (entry.isDirectory())
        {
            Serial.println("/");
            printDirectory(entry, numSpaces + 2);
        }
        else
        {
            // files have sizes, directories do not
            printSpaces(36 - numSpaces - strlen(entry.name()));
            Serial.print("  ");
            Serial.print(entry.size(), DEC);
            DateTimeFields datetime;
            if (entry.getModifyTime(datetime))
            {
                printSpaces(4);
                printTime(datetime);
            }
            Serial.println();
        }
        entry.close();
    }
}
// From https://github.com/PaulStoffregen/LittleFS/blob/main/examples/ListFiles/ListFiles.ino
void littlefs::printSpaces(int num)
{
    for (int i = 0; i < num; i++)
    {
        Serial.print(" ");
    }
}

// From https://github.com/PaulStoffregen/LittleFS/blob/main/examples/ListFiles/ListFiles.ino
void littlefs::printTime(const DateTimeFields tm)
{
    const char *months[12] = {
        "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"};
    if (tm.hour < 10)
        Serial.print('0');
    Serial.print(tm.hour);
    Serial.print(':');
    if (tm.min < 10)
        Serial.print('0');
    Serial.print(tm.min);
    Serial.print("  ");
    Serial.print(tm.mon < 12 ? months[tm.mon] : "???");
    Serial.print(" ");
    Serial.print(tm.mday);
    Serial.print(", ");
    Serial.print(tm.year + 1900);
}