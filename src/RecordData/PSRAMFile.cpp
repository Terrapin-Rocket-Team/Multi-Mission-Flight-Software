#include "PSRAMFile.h"
#include "psram.h"
#include <string.h>

using namespace mmfs;

PSRAMFile::PSRAMFile(const char *name)
{
    setName(name);
}

void PSRAMFile::write(uint8_t *data, int size)
{
    psram->writeFile(*this, (const char *)data, size);
}

void PSRAMFile::print(const char *data)
{
    psram->writeFile(*this, data, strlen(data));
}

void PSRAMFile::println(const char *data)
{
    psram->writeFile(*this, data, strlen(data));
    psram->writeFile(*this, "\n", 1);
}

void PSRAMFile::read(char *data, int size)
{
    psram->readFile(*this, data, size);
}

void PSRAMFile::readTo(char *data, char endChar)
{
// unimplemented
}

void PSRAMFile::setName(const char *name)
{
    strncpy(this->name, name, MAX_FILE_NAME_SIZE);
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
}

