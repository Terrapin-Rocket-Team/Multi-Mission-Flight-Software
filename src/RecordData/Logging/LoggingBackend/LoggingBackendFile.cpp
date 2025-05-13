#include "LoggingBackendFile.h"
#include <string.h>
#include "LoggingBackend.h"

using namespace mmfs;
LoggingBackendFile::LoggingBackendFile(LoggingBackend *b, int fileNum)
    : fileNum(fileNum), b(b)
{
}

size_t LoggingBackendFile::write(const uint8_t *data, size_t len)
{
    return b->write(fileNum, data, len);
}

size_t LoggingBackendFile::write(const char *data, size_t len)
{
    // If len is -1, treat it as null-terminated string
    if (len == static_cast<size_t>(-1))
    {
        len = strlen(data);
    }
    return write((uint8_t *)data, len);
}

size_t LoggingBackendFile::write(const char data)
{
    return write(&data, 1);
}

void LoggingBackendFile::close()
{
    b->close(fileNum);
}

void LoggingBackendFile::save()
{
    b->save(fileNum);
}

int LoggingBackendFile::readBytes(char *dest, size_t len)
{
    return b->read(fileNum, dest, len);
}

size_t LoggingBackendFile::println(const char *data)
{
    size_t written = print(data);
    written += write('\n');
    return written;
}

size_t LoggingBackendFile::print(const char *data)
{
    return write(data);
}
void LoggingBackendFile::seek(long pos){
    b->seek(fileNum, pos);
}

void LoggingBackendFile::timestamp(const char *dateTime)
{
    b->timestamp(fileNum, dateTime);
}
