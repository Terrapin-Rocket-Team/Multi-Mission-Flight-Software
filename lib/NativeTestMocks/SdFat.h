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
#include <cstring>
#include <cstdio>

#define O_RDONLY 0 /* +1 == FREAD */
#define O_WRONLY 1 /* +1 == FWRITE */
#define O_RDWR 2   /* +1 == FREAD|FWRITE */
#define O_APPEND 8
#define O_CREAT 9
#define O_TRUNC 10
#define O_EXCL 11
#define O_SYNC 12

#define T_CREATE 1
#define T_WRITE 2
#define T_ACCESS 4

#define LS_DATE 1
#define LS_SIZE 2

struct FsFileData
{
    char *arr;
    int size;
    int cursor;
    char *name;
};

extern FsFileData *files[10];
class FsFile
{
public:
    FsFileData *data;
    FsFile()
    {
        data = nullptr;
    }
    FsFile(FsFileData *data)
    {
        this->data = data;
    }

    FsFile &operator=(const FsFile &from) // does not copy array
    {
        if (this == &from)
            return *this;
        data = from.data;
        return *this;
    }
    FsFile(const FsFile &from)
    {
        data = from.data;
    }

    ~FsFile()
    {
    }

    void timestamp(int flag, uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
    {
    }
    bool close()
    {
        return true;
    }
    bool write(const char *data, int i)
    {
        FsFileData *d = this->data;
        for (int j = 0; j < i && d->size < 10000; j++)
        {
            d->arr[d->cursor] = data[j];
            if (++(d->cursor) > d->size)
                d->size = d->cursor;
        }
        return true;
    }
    bool print(const char *data)
    {
        write(data, strlen(data));
        return true;
    }
    bool println(const char *data)
    {
        write(data, strlen(data));
        write("\n", 1);
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
    int read(char *dest, int size)
    {
        FsFileData *d = this->data;
        int i = 0;
        while (d->cursor < d->size && size > 0)
        {
            dest[size] = d->arr[d->cursor];
            d->cursor++;
            size--;
            i++;
        }
        return i;
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
    int readBytes(char *c, int len) { return 0; }
};

class SdFs
{
public:
    SdFs()
    {
        for (int i = 0; i < 10; i++)
        {
            files[i] = nullptr;
        }
    }
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
        for (int i = 0; i < 10; i++)
        {
            if (files[i] && files[i]->name && strcmp(files[i]->name, fileName) == 0)
            {
                return true;
            }
        }
        return false;
    }
    FsFile open(const char *fileName, int mode)
    {
        FsFile f;
        for (int i = 0; i < 10; i++)
        {
            if (files[i] && files[i]->name && strcmp(files[i]->name, fileName) == 0)
            {
                f.data = files[i];
                return f;
            }
        }
        FsFileData *d = new FsFileData();
        d->arr = new char[10000];
        d->size = 0;
        d->cursor = 0;
        d->name = new char[strlen(fileName) + 1];
        strcpy(d->name, fileName);
        for (int i = 0; i < 10; i++)
        {
            if (files[i] == nullptr)
            {
                files[i] = d;
                break;
            }
        }
        f.data = d;
        return f;
    }
    ~SdFs()
    {
        for (int i = 0; i < 10; i++)
        {
            if (files[i])
            {
                delete[] files[i]->arr;
                delete[] files[i]->name;
                delete files[i];
            }
            files[i] = nullptr;
        }
    }
    void ls(int i)
    {
    }
    void format() {}
    void remove(char *c){}
};

#endif