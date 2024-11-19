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

class FsFile
{
public:
    FsFile()
    {
        arr = new char[10000]; // arbitrary size
        size = new int(0);
        cursor = new int(0);
        name = nullptr;
    }
    FsFile(const char *name)
    {
        arr = new char[10000]; // arbitrary size
        size = new int(0);
        cursor = new int(0);
        this->name = new char[strlen(name) + 1];
        snprintf(this->name, strlen(name) + 1, "%s", name);
    }

    FsFile &operator=(const FsFile &from) // does not copy array
    {
        if (this == &from)
            return *this;
        size = from.size;
        cursor = from.cursor;
        name = from.name;
        arr = from.arr; // This is a memory leak but idc for unit testing
        return *this;
    }
    FsFile(const FsFile &from)
    {
        size = from.size;
        cursor = from.cursor;
        name = from.name;
        arr = from.arr;
    }
    ~FsFile()
    {
    }
    bool close()
    {
        return true;
    }
    bool write(const char *data, int i)
    {
        for (int j = 0; j < i && *size < 10000; j++)
        {
            arr[*cursor] = data[j];
            if (++(*cursor) > *size)
                *size = *cursor;
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
        int i = 0;
        while(*cursor < *(this->size) && size > 0)
        {
            dest[size] = arr[*cursor];
            (*cursor)++;
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
    char *arr;
    int *size;
    int *cursor;
    char *name;
};

class SdFs
{
public:
    SdFs()
    {
        files = new FsFile*[10];
        for(int i = 0; i < 10; i++)
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
        for(int i = 0; i < 10; i++)
        {
            if(files[i] != nullptr && strcmp(files[i]->name, fileName) == 0)
            {
                return true;
            }
        }
        return false;
    }
    FsFile open(const char *fileName, int mode)
    {
        for(int i = 0; i < 10; i++)
        {
            if (files[i] != nullptr && strcmp(files[i]->name, fileName) == 0)
            {
                return *files[i];
            }
        }
        FsFile *fs = new FsFile(fileName);
        for(int i = 0; i < 10; i++)
        {
            if (files[i] == nullptr || files[i]->name == nullptr)
            {
                files[i] = fs;
                return *fs;
            }
        }
        return nullptr;
    }
    FsFile **files;
};

#endif