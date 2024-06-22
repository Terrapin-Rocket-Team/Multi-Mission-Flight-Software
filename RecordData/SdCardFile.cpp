#include "SdCardFile.h"

SdCardFile::SdCardFile()
{
    sdReady = false;
}

bool SdCardFile::init()
{
    int rdy = 0;
    sdReady = false;
    if (sd.begin(SD_CONFIG) || sd.restart())
    {
        // Find file name
        int fileNo = 0;
        bool exists = true;
// Disable the warning for the following line, because seeing the warning all the time is annoying
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-truncation"
        while (exists)
        {
            snprintf(fileName, NAME_SIZE, "%d_Log.txt", ++fileNo);
            exists = sd.exists(fileName);
        }
#pragma GCC diagnostic pop

        // Setup files
        file = sd.open(fileName, FILE_WRITE);
        if (file)
        {
            file.close();
            rdy++;
        }
    }
    sdReady = rdy == 2;

    return sdReady;
}

bool SdCardFile::isReady()
{
    return sdReady;
}

void SdCardFile::print(const char *data)
{
    if (sdReady)
    {
        file.open(fileName, FILE_WRITE);
        file.print(data);
        file.close();
    }
}

void SdCardFile::println(const char *data)
{
    if (sdReady)
    {
        file.open(fileName, FILE_WRITE);
        file.println(data);
        file.close();
    }
}

char *SdCardFile::read(char *data, int size)
{
    if (sdReady)
    {
        file.open(fileName, FILE_READ);
        file.read(data, size);
        file.close();
    }
    return data;
}

char *SdCardFile::readTo(char *data, char endChar)
{
    if (sdReady)
    {
        file.open(fileName, FILE_READ);
        
        while (file.available() && file.peek() != endChar)
        {
            *data = file.read();
            data++;
        }
        
        file.close();
    }
    return data;
}

bool SdCardFile::seek(uint64_t offset)
{
    if (sdReady)
    {
        file.open(fileName, FILE_READ);
        file.seek(offset);
        file.close();
    }
    return true;
}
