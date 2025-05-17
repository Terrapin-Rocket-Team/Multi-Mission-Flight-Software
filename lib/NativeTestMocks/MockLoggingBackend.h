#ifndef LOGGING_BACKEND_MOCK_H
#define LOGGING_BACKEND_MOCK_H

#include "../../src/RecordData/Logging/LoggingBackend/LoggingBackend.h"
#include <algorithm>
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
        size_t size, cur;

        MockFileData() : size(0), cur(0)
        {
            memset(arr, 0, sizeof(arr));
        }

        void write(const char *data, size_t len)
        {
            if (cur + len > MOCK_FILE_SIZE)
                len = MOCK_FILE_SIZE - cur;
            size = ::std::max(cur + len, size);
            memcpy(arr + cur, data, len);
            cur += len;
        }
        int readBytes(char *dest, size_t len)
        {
            if (len > size - cur)
                len = size - cur;
            memcpy(dest, arr + cur, len);
            cur += len;
            return len;
        }
        void clear()
        {
            size = cur = 0;
            memset(arr, 0, sizeof(arr));
        }
        void seek(size_t pos)
        {
            cur = ::std::min(size, ::std::max((size_t)0, pos));
        }
    };

#endif

    class LoggingBackendFileMock
    {
    public:
        MockFileData *data;
        LoggingBackendFileMock(MockFileData *data) : data(data) {}

        size_t write(const uint8_t *d, size_t len)
        {
            data->write(reinterpret_cast<const char *>(d), len);
            return len;
        }

        size_t write(const char *d, size_t len = -1)
        {
            if (len == static_cast<size_t>(-1))
                len = strlen(d);
            data->write(d, len);
            return len;
        }

        size_t write(const char d)
        {
            data->write(&d, 1);
            return 1;
        }

        void close() {}
        void save() {}
        int readBytes(char *dest, size_t len)
        {
            if (len > data->size)
                len = data->size;
            memcpy(dest, data->arr, len);
            return len;
        }

        size_t println(const char *d)
        {
            size_t written = write(d);
            written += write('\n');
            return written;
        }

        size_t print(const char *d)
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

        LoggingBackendFile *open(const char *filename, uint8_t flags) override
        {
            for (int i = 0; i < MAX_FILES; ++i)
            {
                if (fileExists[i] && strcmp(filename, filenames[i]) == 0)
                {
                    if (flags == FI_WRITE_BEGINNING || flags == FI_READ)
                        files[i].seek(0);
                    else
                        files[i].seek(files[i].size);
                    return new LoggingBackendFile(this, i);
                }
            }
            for (int i = 0; i < MAX_FILES; ++i)
            {
                if (!fileExists[i])
                {
                    strcpy(filenames[i], filename);
                    fileExists[i] = true;
                    files[i].clear();
                    return new LoggingBackendFile(this, i);
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

        void ls(Stream &s) override {}
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

        void seek(int file, long pos) override
        {
            if (fileExists[file])
                files[file].seek(pos);
        }

        MockFileData *getMockFileData(const char *name)
        {
            for (int i = 0; i < MAX_FILES; ++i)
            {
                if (fileExists[i] && strcmp(filenames[i], name) == 0)
                {
                    return &files[i];
                }
            }
            return nullptr;
        }
    };

}

#endif
