#ifndef MOCK_LOGGING_BACKEND_H
#define MOCK_LOGGING_BACKEND_H

#include <cstdint>
#include <cstring>
#include <cstdio>

#include "../../src/RecordData/Logging/LoggingBackend/LoggingBackend.h"

namespace mmfs
{

    struct MockFileData
    {
        char *arr;
        int size;
        int cursor;
        char *name;
    };

    extern MockFileData *files[10];
    class MockFile
    {
    public:
        MockFileData *data;
        MockFile()
        {
            data = nullptr;
        }
        MockFile(MockFileData *data)
        {
            this->data = data;
        }

        MockFile &operator=(const MockFile &from) // does not copy array
        {
            if (this == &from)
                return *this;
            data = from.data;
            return *this;
        }
        MockFile(const MockFile &from)
        {
            data = from.data;
        }

        ~MockFile()
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
            MockFileData *d = this->data;
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
            MockFileData *d = this->data;
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

    class MockLoggingBackend : public LoggingBackend
    {
    public:
        MockLoggingBackend()
        {
            for (int i = 0; i < 10; i++)
            {
                files[i] = nullptr;
            }
        }
        bool begin() override
        {
            return true;
        }
        bool exists(const char *fileName) override
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
        LoggingBackendFile *open(const char *fileName) override
        {
            MockFile f;
            for (int i = 0; i < 10; i++)
            {
                if (files[i] && files[i]->name && strcmp(files[i]->name, fileName) == 0)
                {
                    f.data = files[i];
                    return new LoggingBackendFile(this, i);
                }
            }
            MockFileData *d = new MockFileData();
            d->arr = new char[10000];
            d->size = 0;
            d->cursor = 0;
            d->name = new char[strlen(fileName) + 1];
            strcpy(d->name, fileName);
            int i = 0;
            for (i; i < 10; i++)
            {
                if (files[i] == nullptr)
                {
                    files[i] = d;
                    break;
                }
            }
            f.data = d;
            return new LoggingBackendFile(this, i);
        }
        void close(int file) override {}
        size_t read(int file, char *dest, size_t len) override { return 0; }
        bool isAvailable() override {return true;}
        void save(int file) override {}
        size_t write(int file, const uint8_t *data, size_t len) override { return 0;}
        ~MockLoggingBackend()
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
        void ls(int i) override
        {
        }
        void format() override {}
        bool remove(const char *c) override { return true; }
    };
} // namespace mmfs
#endif