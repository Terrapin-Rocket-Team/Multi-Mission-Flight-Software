#ifndef LOGGING_BACKEND_MOCK_H
#define LOGGING_BACKEND_MOCK_H

#include "../../src/RecordData/Logging/LoggingBackend/LoggingBackend.h"

namespace mmfs
{
#ifndef MOCK_FILE_DATA_H
#define MOCK_FILE_DATA_H

#include <cstring>
#include <cstdint>

#define MOCK_FILE_SIZE 4096 // Adjust as needed

    struct MockFileData
    {
        char arr[MOCK_FILE_SIZE];
        size_t size;

        MockFileData() : size(0)
        {
            memset(arr, 0, sizeof(arr));
        }

        void write(const char *data, size_t len)
        {
            if (size + len > MOCK_FILE_SIZE)
                len = MOCK_FILE_SIZE - size;
            memcpy(arr + size, data, len);
            size += len;
        }
        int readBytes(char *dest, size_t len)
        {
            if (len > size)
                len = size;
            memcpy(dest, arr, len);
            return len;
        }
        void clear()
        {
            size = 0;
            memset(arr, 0, sizeof(arr));
        }
    };

#endif

    class LoggingBackendFileMock : public LoggingBackendFile
    {
    public:
        MockFileData *data;

        LoggingBackendFileMock(MockFileData *data) : LoggingBackendFile(nullptr, 0), data(data) {}

        size_t write(const uint8_t *d, size_t len) override
        {
            data->write(reinterpret_cast<const char *>(d), len);
            return len;
        }

        size_t write(const char *d, size_t len = -1) override
        {
            if (len == static_cast<size_t>(-1))
                len = strlen(d);
            data->write(d, len);
            return len;
        }

        size_t write(const char d) override
        {
            data->write(&d, 1);
            return 1;
        }

        void close() override {}
        void save() override {}
        int readBytes(char *dest, size_t len) override
        {
            if (len > data->size)
                len = data->size;
            memcpy(dest, data->arr, len);
            return len;
        }

        size_t println(const char *d) override
        {
            size_t written = write(d);
            written += write('\n');
            return written;
        }

        size_t print(const char *d) override
        {
            return write(d);
        }
    };

    class LoggingBackendMock : public LoggingBackend
    {
    public:
        MockFileData files[MAX_FILES];
        bool fileExists[MAX_FILES];
        char *filenames[MAX_FILES];

        LoggingBackendMock()
        {
            for (int i = 0; i < MAX_FILES; ++i)
            {
                fileExists[i] = false;
                filenames[i] = new char[64];
            }
        }

        bool begin() override
        {
            rdy = true;
            return true;
        }

        LoggingBackendFile *open(const char *filename) override
        {
            for (int i = 0; i < MAX_FILES; ++i)
            {
                if (fileExists[i] && strcmp(filename, filenames[i]) == 0)
                {
                    return new LoggingBackendFileMock(&files[i]);
                }
            }
            for (int i = 0; i < MAX_FILES; ++i)
            {
                if (!fileExists[i])
                {
                    strcpy(filenames[i], filename);
                    fileExists[i] = true;
                    files[i].clear();
                    return new LoggingBackendFileMock(&files[i]);
                }
            }
            return nullptr;
        }

        void close(int file) override {}
        void save(int file) override {}

        size_t read(int file, char *dest, size_t len) override
        {
            if (!fileExists[file])
                return 0;
            return files[file].readBytes(dest, len);
        }

        size_t write(int file, const uint8_t *data, size_t len) override
        {
            if (!fileExists[file])
                return 0;
            files[file].write(reinterpret_cast<const char *>(data), len);
            return len;
        }

        bool exists(const char *filename) override
        {
            for (int i = 0; i < MAX_FILES; ++i)
            {
                if (fileExists[i] && strcmp(filenames[i], filename) == 0)
                    return true;
            }
            return false;
        }

        bool isAvailable() override
        {
            return rdy;
        }

        void ls(int i = -1) override {}
        void format() override
        {
            for (int i = 0; i < MAX_FILES; ++i)
            {
                fileExists[i] = false;
                filenames[i] = nullptr;
                files[i].clear();
            }
        }

        bool remove(const char *filename) override
        {
            for (int i = 0; i < MAX_FILES; ++i)
            {
                if (fileExists[i] && strcmp(filenames[i], filename) == 0)
                {
                    fileExists[i] = false;
                    filenames[i] = nullptr;
                    files[i].clear();
                    return true;
                }
            }
            return false;
        }
    };

}

#endif
