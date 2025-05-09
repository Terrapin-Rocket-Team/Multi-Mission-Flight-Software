#ifndef LOGGING_BACKEND_FILE_H
#define LOGGING_BACKEND_FILE_H

#include <cstdint>
#include <cstddef>
namespace mmfs
{
    class LoggingBackend;
    class LoggingBackendFile
    {

    public:
        LoggingBackendFile(LoggingBackend *b, int fileNum);
        virtual size_t write(const uint8_t *data, size_t len);
        virtual size_t write(const char *data, size_t len = -1);
        virtual size_t write(const char data);
        virtual void close();
        virtual void save();
        virtual int readBytes(char *dest, size_t len);
        virtual size_t println(const char *data);
        virtual size_t print(const char *data);
        virtual void seek(long pos);

    protected:
        int fileNum;
        LoggingBackend *b;
    };

} // namespace mmfs
#endif