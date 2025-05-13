#include "LoggingBackendSdFat.h"

#include "../SdFatBoilerplate.h"

using namespace mmfs;

namespace mmfs::littlefs // declare some helper functions used at the end
{
    void printDirectory(Stream &s, FsFile dir, int numSpaces);
    void printTime(Stream &s, const DateTimeFields tm);
} // namespace littlefs

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
    return rdy = sdfs != nullptr && (sdfs->begin(SD_CONFIG) || sdfs->restart());
}

LoggingBackendFile *LoggingBackendSdFat::open(const char *filename)
{
    unsigned int i = 0;
    while (activeFiles[i] && i < MAX_FILES) // if exists
    {
        char fname[250];
        activeFiles[i]->getName(fname, 250);
        if (!strcmp(fname, filename))
        {
            return new LoggingBackendFile(this, i);
        }
        i++;
    }
    // if doesnt exist
    if (i < MAX_FILES)
    {
        FsFile *f = new FsFile(sdfs->open(filename, FILE_WRITE | FILE_READ));
        if (f)
        {
            activeFiles[i] = f;
            return new LoggingBackendFile(this, i);
        }
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

void LoggingBackendSdFat::close(int file)
{
    if (activeFiles[file])
    {
        activeFiles[file]->close();
        activeFiles[file] = nullptr;
    }
}

void LoggingBackendSdFat::save(int file)
{
    if (activeFiles[file])
    {
        activeFiles[file]->flush();
    }
}

void LoggingBackendSdFat::ls(Stream &s)
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
        return activeFiles[file]->readBytes(dest, len);
    return 0;
}

void LoggingBackendSdFat::seek(int file, long pos)
{
    if (activeFiles[file])
        activeFiles[file]->seek(pos);
}

// From https://github.com/PaulStoffregen/LittleFS/blob/main/examples/ListFiles/ListFiles.ino
void littlefs::printDirectory(Stream &s, FsFile dir, int numSpaces)
{
    while (true)
    {
        FsFile entry = dir.openNextFile();
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
            uint16_t d = 0, t = 0;
            if (entry.getModifyDateTime(&d, &t))
            {
                if(!fsPrintDateTime(&s, d, t))
                    s.print("N/A");
            }
            s.println();
        }
        entry.close();
    }
}