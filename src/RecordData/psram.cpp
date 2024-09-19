#include "psram.h"

using namespace mmfs;


PSRAM::PSRAM()
{
    ready = false;
    dumped = false;

    cursorStart = nullptr;
    cursorEnd = nullptr;
    memBegin = nullptr;
    memEnd = nullptr;
}

bool PSRAM::init()
{
    uint8_t size = external_psram_size;
    memBegin = cursorStart = reinterpret_cast<char *>(0x70000000);
    memEnd = cursorEnd = cursorStart + (size * 1048576);

    if (size > 0)
    {
        ready = true;
    }

    return ready;
}

void PSRAM::write(char *data, int size)
{
    write(data, size, true);
}

void PSRAM::write(char *data, int size, bool writeToTop)
{
    if (ready)
    {
        if (writeToTop)
        {
            for (int i = 0; i < size; i++)
            {
                *cursorStart = data[i];
                cursorStart++;
            }
        }
        else
        {
            for (int i = 0; i < size; i++)
            {
                *cursorEnd = data[i];
                cursorEnd--;
            }
        }
    }
}

void PSRAM::println(const char *data, bool writeToTop)
{
    if (ready)
    {
        print(data, writeToTop);
        print("\n", writeToTop);
    }
}

// Write string to FRAM
void PSRAM::print(const char *data, bool writeToTop)
{
    if (ready) {
        for (int i = 0; data[i] != '\0'; i++)
        {
            if (writeToTop)
            {
                *cursorStart = data[i];
                cursorStart++;
            }
            else
            {
                *cursorEnd = data[i];
                cursorEnd--;
            }
        }
    }
}

// Returns whether the FRAM is initialized
bool PSRAM::isReady() const
{
    return ready;
}

int PSRAM::getFreeSpace() const
{
    return cursorEnd - cursorStart;
}

int PSRAM::getDataSpace() const
{
    return cursorStart - memBegin;
}

int PSRAM::getLogSpace() const
{
    return memEnd - cursorEnd;
}

int PSRAM::read(char *data, int size)
{
    int i = -1;
    if (ready)
    {
        char *mem = memBegin;
        i = 0;
        while (mem != cursorStart && i < size)
        {
            *data = *mem;
            mem++;
            data++;
            i++;
        }
    }
    return i;
}

int PSRAM::read(char *data)
{
    int i = -1;
    if (ready)
    {
        i = 0;
        char *mem = memBegin;
        while (mem != cursorStart)
        {
            *data = *mem;
            mem++;
            data++;
            i++;
        }
    }
    return i;
}

int PSRAM::readTo(char *data, char endChar)
{
    int i = -1;
    if (ready)
    {
        i = 0;
        char *mem = memBegin;
        while (*mem != endChar && mem != cursorStart)
        {
            *data = *mem;
            mem++;
            data++;
            i++;
        }
    }
    return i;
}

int PSRAM::readFromBottom(char *data, int size)
{
    int i = -1;
    if (ready)
    {
        char *mem = memEnd;
        i = 0;
        while (i < size && mem != cursorEnd) {
            *data = *mem;
            mem--;
            data++;
            i++;
        }
    }

    return i;
}

int PSRAM::readFromBottom(char *data)
{
    int i = -1;
    if (ready)
    {
        i = 0;
        char *mem = memEnd;
        while (mem != cursorEnd)
        {
            *data = *mem;
            mem--;
            data++;
            i++;
        }
    }

    return i;
}

bool PSRAM::seek(uint64_t offset)
{
    if (ready)
    {
        cursorStart += offset;
        return true;
    }
    return false;
}

bool PSRAM::seekFromBottom(uint64_t offset)
{
    if (ready)
    {
        cursorEnd -= offset;
        return true;
    }
    return false;
}