#include "PSRAMFile.h"
#include "psram.h"
#include <string.h>

using namespace mmfs;

PSRAMFile::PSRAMFile(const char *name, PSRAM *psram)
{
    setName(name);
    this->psram = psram;
}

PSRAMFile::~PSRAMFile()
{
    delete[] name;
}

void PSRAMFile::write(const uint8_t *data, int size)
{
    if (!_open || openMode == F_READ)
        return;
    psram->writeFile(*this, (const char *)data, size);
}

void PSRAMFile::write(const char *data, int size)
{
    if (!_open || openMode == F_READ)
        return;
    psram->writeFile(*this, data, size);
}

void PSRAMFile::print(const char *data)
{
    if (!_open || openMode == F_READ)
        return;
    psram->writeFile(*this, data, strlen(data)); // TODO: avoid 2x loops using a while loop

}

void PSRAMFile::println(const char *data)
{
    if (!_open || openMode == F_READ)
        return;
    psram->writeFile(*this, data, strlen(data));
    psram->writeFile(*this, "\n", 1);
}

int PSRAMFile::read(char *data, int size)
{
    if (!_open || !(openMode & F_READ))
        return 0;
    int i = psram->readFile(*this, data, size);
    return i;
}

void PSRAMFile::readTo(char *data, char endChar)
{
    // unimplemented
}

void PSRAMFile::setName(const char *name)
{
    snprintf(this->name, MAX_FILE_NAME_SIZE, "%s", name);
}

const char *PSRAMFile::getName()
{
    return name;
}

int PSRAMFile::getSize()
{
    return size;
}

void PSRAMFile::restart()
{
    clusterCursor = startCluster;
    cursor = startCluster * PSRAM_CLUSTER_SIZE;
}

void PSRAMFile::toEnd()
{
    cursor = endOfFile;
    clusterCursor = eofCluster;
}

void PSRAMFile::seek(int offset, uint8_t origin)
{
    psram->seekFile(*this, offset, origin);
}

void PSRAMFile::close()
{
    _open = false;
}

void PSRAMFile::open(uint8_t mode)
{
    openMode = mode;
    if (mode & F_APPEND)
    {
        toEnd();
    }
    else if ((mode & F_READ) || (mode & F_WRITE))
    {
        restart();
    }
    _open = true;
}

bool PSRAMFile::isOpen() const
{
    return _open;
}

int PSRAMFile::getCursor() const
{
    return cursor;
}
