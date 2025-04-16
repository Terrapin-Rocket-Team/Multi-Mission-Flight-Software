#ifndef LOGGING_BACKEND_SDFAT
#define LOGGING_BACKEND_SDFAT

#include "LoggingBackend.h"
#include "../SdFatBoilerplate.h"
namespace mmfs
{
    class LoggingBackendSdFat : public LoggingBackend
    {
    public:
        LoggingBackendSdFat();
        virtual bool begin() override;
        virtual LoggingBackendFile *open(const char *filename) override;
        virtual size_t write(int file, const uint8_t *data, size_t len) override;
        virtual bool isAvailable() override;
        virtual bool exists(const char *filename) override;
        virtual ~LoggingBackendSdFat();

    private:
        SdFs *sdfs;
        FsFile *activeFiles[MAX_FILES]; // max of 10 files
    };
}
#endif