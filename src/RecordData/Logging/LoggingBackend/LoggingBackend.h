#ifndef LOGGER_BACKEND_H
#define LOGGER_BACKEND_H

#include <stdint.h>
#include <cstddef>
#include "../../../Constants.h"
#include "LoggingBackendFile.h"

class Stream;

namespace mmfs
{
    class LoggingBackend
    {
    public:
        LoggingBackend() {}
        virtual bool begin() = 0;
        virtual LoggingBackendFile *open(const char *filename) = 0;
        virtual void close(int file) = 0;
        virtual void save(int file) = 0;
        virtual size_t read(int file, char *dest, size_t len) = 0;
        virtual size_t write(int file, const uint8_t *data, size_t len) = 0;
        virtual bool exists(const char *filename) = 0;
        virtual bool isAvailable() = 0;
        virtual void ls(Stream &s) = 0;
        virtual void format() = 0;
        virtual bool remove(const char *filename) = 0;
        virtual ~LoggingBackend() {}
        virtual void seek(int file, long pos) = 0;

    protected:
        bool rdy = false;
    };
}
#endif