#ifndef SDFAT_H
#define SDFAT_H

#define FILE_WRITE 1
#define FILE_READ 0
#define SHARED_SPI 10
#define SPI_HALF_SPEED 10
#define SPI_FULL_SPEED 10
#define SD_SCK_MHZ(int) (10)

#define SdSpiConfig(int1, int2, int3) 1

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