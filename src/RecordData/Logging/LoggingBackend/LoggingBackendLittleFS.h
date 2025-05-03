#ifndef LOGGING_BACKEND_LITTLEFS
#define LOGGING_BACKEND_LITTLEFS

#include "LoggingBackend.h"

class LittleFS_QPINAND;
class File;
namespace mmfs
{

    class LoggingBackendLittleFS : public LoggingBackend
    {
    public:
        LoggingBackendLittleFS();
        virtual bool begin() override;
        virtual LoggingBackendFile *open(const char *filename) override;
        virtual void close(int file) override;
        virtual void save(int file) override;
        virtual size_t read(int file, char *dest, size_t len) override;
        virtual size_t write(int file, const uint8_t *data, size_t len) override;
        virtual bool isAvailable();
        virtual bool exists(const char *filename) override;
        virtual void ls(int i = -1) override;
        virtual void format() override;
        virtual bool remove(const char *filename) override;
        virtual void seek(int file, long pos) override;
        virtual ~LoggingBackendLittleFS();

    private:
        LittleFS_QPINAND *lfs;
        File *activeFiles[MAX_FILES];
    };

}
#endif