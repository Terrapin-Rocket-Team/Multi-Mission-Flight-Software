#ifndef SDFAT_H
#define SDFAT_H

#define FILE_WRITE 1
#define FILE_READ 0
#define SHARED_SPI 10
#define SPI_HALF_SPEED 10
#define SPI_FULL_SPEED 10
#define SD_SCK_MHZ(int) (10)

#define SdSpiConfig(int1, int2, int3) 1

#include <cstdint>

#define O_RDONLY 0 /* +1 == FREAD */
#define O_WRONLY 1 /* +1 == FWRITE */
#define O_RDWR 2   /* +1 == FREAD|FWRITE */
#define O_APPEND 8
#define O_CREAT 9
#define O_TRUNC 10
#define O_EXCL 11
#define O_SYNC 12

class FsFile
{
public:
    bool close()
    {
        return true;
    }
    bool write(const char *data)
    {
        return true;
    }
    bool write(const char *data, int i)
    {
        return true;
    }
    bool print(const char *data)
    {
        return true;
    }
    bool println(const char *data)
    {
        return true;
    }
    explicit operator bool() const
    {
        return true;
    }
    bool open(const char *fileName, int mode)
    {
        return true;
    }
    bool seek(uint64_t offset)
    {
        return true;
    }
    int read(char *data, int size)
    {
        return 0;
    }
    bool available()
    {
        return true;
    }
    int peek()
    {
        return 0;
    }
    int read()
    {
        return 0;
    }
};

class SdFs
{
public:
    bool begin(int config)
    {
        return true;
    }
    bool restart()
    {
        return true;
    }
    bool exists(const char *fileName)
    {
        return true;
    }
    FsFile open(const char *fileName, int mode)
    {
        return FsFile();
    }
};

#endif