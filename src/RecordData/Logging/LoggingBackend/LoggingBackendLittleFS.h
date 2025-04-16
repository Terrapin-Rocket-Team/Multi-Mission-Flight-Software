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
        virtual bool begin() override;
        virtual LoggingBackendFile *open(const char *filename) override;
        virtual size_t write(int file, const uint8_t *data, size_t len) override;
        virtual bool isAvailable();
        virtual bool exists(const char *filename) override;
        virtual ~LoggingBackendLittleFS();

    private:
        LittleFS_QPINAND *lfs;
        File *activeFiles[MAX_FILES]; // max of 10 files
    };
}
#endif