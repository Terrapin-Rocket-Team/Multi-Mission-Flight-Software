#ifndef LOGGING_BACKEND_SDFAT
#define LOGGING_BACKEND_SDFAT

#include "LoggingBackend.h"
#include "../SdFatBoilerplate.h"
namespace mmfs
{
    class LoggingBackendSdFat : public LoggingBackend
    {
    public:
        LoggingBackendSdFat::LoggingBackendSdFat();
        virtual bool begin();
        virtual bool open(const char *filename);
        virtual size_t write(int file, const uint8_t *data, size_t len);
        virtual size_t write(const char *filename, const uint8_t *data, size_t len);
        virtual void close();
        virtual bool isAvailable();
        virtual bool exists(const char *filename);
        virtual void save(const char *filename);
        virtual void save(int file);
        virtual ~LoggingBackendSdFat();

    private:
        SdFs *sd;
        FsFile *activeFiles[MAX_FILES]; // max of 10 files
    };
}
#endif