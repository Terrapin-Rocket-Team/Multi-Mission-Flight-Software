#ifndef LOGGING_BACKEND_FILE_H
#define LOGGING_BACKEND_FILE_H

#include <cstdint>
#include <cstddef>
#include <Print.h>
namespace mmfs
{
    class LoggingBackend;
    class LoggingBackendFile : public Print
    {

    public:
        LoggingBackendFile(LoggingBackend *b, int fileNum);
        virtual size_t write(uint8_t byte) override;
        virtual void close();
        virtual void save();
        virtual int readBytes(char *dest, size_t len);
        virtual size_t println(const char *data);
        virtual size_t print(const char *data);
        virtual void seek(long pos);
        virtual void timestamp(const char *dateTime);

    protected:
        int fileNum;
        LoggingBackend *b;
    };

} // namespace mmfs
#endif