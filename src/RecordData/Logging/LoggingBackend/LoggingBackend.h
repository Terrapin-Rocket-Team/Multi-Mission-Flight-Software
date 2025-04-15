#ifndef LOGGER_BACKEND_H
#define LOGGER_BACKEND_H

#include <stdint.h>
#include <cstddef>
#include "../../../Constants.h"
namespace mmfs
{
    class LoggingBackend
    {
    public:
        virtual bool begin() = 0;
        virtual bool open(const char *filename) = 0;
        virtual size_t write(int file, const uint8_t *data, size_t len) = 0;
        virtual size_t write(const char *filename, const uint8_t *data, size_t len) = 0;
        virtual void close() = 0;
        virtual bool isAvailable() = 0;
        virtual ~LoggingBackend() {}

    protected:
        LoggingBackend();
        virtual void findFileNum() = 0;
        bool rdy = false;
    };
}
#endif