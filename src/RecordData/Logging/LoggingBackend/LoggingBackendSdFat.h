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
        virtual LoggingBackendFile *open(const char *filename, uint8_t flags) override;
        virtual void close(int file) override;
        virtual void save(int file) override;
        virtual size_t read(int file, char *dest, size_t len) override;
        virtual size_t write(int file, const uint8_t *data, size_t len) override;
        virtual bool isAvailable() override;
        virtual bool exists(const char *filename) override;
        virtual void ls(Stream &s) override;
        virtual void format() override;
        virtual bool remove(const char *filename) ;
        virtual ~LoggingBackendSdFat();
        virtual void seek(int file, long pos) override;
        virtual void timestamp(int file, const char *dateTime) override;

    private:
        SdFs *sdfs;
        FsFile activeFiles[MAX_FILES]; // max of 10 files
    };
}
#endif