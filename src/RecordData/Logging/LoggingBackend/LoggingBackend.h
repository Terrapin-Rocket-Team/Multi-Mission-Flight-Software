#ifndef LOGGER_BACKEND_H
#define LOGGER_BACKEND_H

#include <stdint.h>
#include <cstddef>
#include "../../../Constants.h"
#include "LoggingBackendFile.h"
namespace mmfs
{
    enum FIO
    {
        FI_READ,           // read only
        FI_WRITE,          // write from the end of the file, appending content
        FI_WRITE_BEGINNING // write from the start of the file, overwriting as you go
    };
    class LoggingBackend
    {
    public:
        LoggingBackend() {}
        virtual bool begin() = 0;
        virtual LoggingBackendFile *open(const char *filename, uint8_t rw = FI_READ) = 0;
        virtual void close(int file) = 0;
        virtual void save(int file) = 0;
        virtual size_t read(int file, char *dest, size_t len) = 0;
        virtual size_t write(int file, const uint8_t *data, size_t len) = 0;
        virtual bool exists(const char *filename) = 0;
        virtual bool isAvailable() = 0;
        virtual void ls(int i = -1) = 0;
        virtual void format() = 0;
        virtual bool remove(const char *filename) = 0;
        virtual ~LoggingBackend() {}
        virtual void seek(int file, long pos) = 0;

    protected:
        bool rdy = false;
    };
}
#endif