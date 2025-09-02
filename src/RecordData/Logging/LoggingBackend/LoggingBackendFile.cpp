#include "LoggingBackendFile.h"
#include <string.h>
#include "LoggingBackend.h"

using namespace mmfs;
LoggingBackendFile::LoggingBackendFile(LoggingBackend *b, int fileNum)
    : fileNum(fileNum), b(b)
{
}

size_t LoggingBackendFile::write(uint8_t byte)
{
    return b->write(fileNum, &byte, 1);
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
void LoggingBackendFile::seek(long pos){
    b->seek(fileNum, pos);
}

void LoggingBackendFile::timestamp(const char *dateTime)
{
    b->timestamp(fileNum, dateTime);
}
