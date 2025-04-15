#ifndef LOGGING_BACKEND_LITTLEFS
#define LOGGING_BACKEND_LITTLEFS

#include "LoggingBackend.h"
#include <LittleFS.h>
namespace mmfs
{
    class LoggingBackendLittleFS : public LoggingBackend
    {
    public:
        LoggingBackendLittleFS::LoggingBackendLittleFS();
        virtual bool begin();
        virtual bool open(const char *filename);
        virtual size_t write(int file, const uint8_t *data, size_t len);
        virtual size_t write(const char *filename, const uint8_t *data, size_t len);
        virtual void close();
        virtual bool isAvailable();
        virtual bool exists(const char *filename);
        virtual void save(const char *filename);
        virtual void save(int file);
        virtual ~LoggingBackendLittleFS();

    private:
        LittleFS_QPINAND *lfs;
        File *activeFiles[MAX_FILES]; // max of 10 files
    };
}
#endif