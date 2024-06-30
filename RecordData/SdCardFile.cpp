#include "SdCardFile.h"

SdCardFile::SdCardFile()
{
    sdReady = false;
}

bool SdCardFile::init()
{
    init("FlightData.csv");
}

bool SdCardFile::init(const char *fileSuffix)
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
            snprintf(fileName, NAME_SIZE, "%d_%s", ++fileNo, fileSuffix);
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

bool SdCardFile::isReady() const
{
    return sdReady;
}

void SdCardFile::write(char *data, int size)
{
    if (sdReady)
    {
        file.open(fileName, FILE_WRITE);
        file.write(data, size);
        file.close();
    }
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

int SdCardFile::read(char *data, int size)
{
    int i;
    if (sdReady)
    {
        file.open(fileName, FILE_READ);
        i = file.read(data, size);
        file.close();
    }
    return i;
}

int SdCardFile::readTo(char *data, char endChar)
{
    int i = -1;
    if (sdReady)
    {
        file.open(fileName, FILE_READ);

        i = 0;
        
        while (file.available() && file.peek() != endChar)
        {
            *data = file.read();
            data++;
            i++;
        }
        
        file.close();
    }
    return i;
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
