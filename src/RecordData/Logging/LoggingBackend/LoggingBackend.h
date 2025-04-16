#ifndef LOGGER_BACKEND_H
#define LOGGER_BACKEND_H

#include <stdint.h>
#include <cstddef>
#include "../../../Constants.h"
#include "LoggingBackendFile.h"
namespace mmfs
{
    class LoggingBackend
    {
    public:
        virtual bool begin() = 0;
        virtual LoggingBackendFile *open(const char *filename) = 0;
        virtual size_t write(int file, const uint8_t *data, size_t len) = 0;
        virtual bool exists(const char *filename) = 0;
        virtual bool isAvailable() = 0;
        virtual void ls(int i = -1);
        virtual void format();
        virtual bool remove(const char *filename);
        virtual ~LoggingBackend() {}

    protected:
        LoggingBackend();
        bool rdy = false;
        LoggingBackendFile activeFilesDup[MAX_FILES];
    };
}
#endif