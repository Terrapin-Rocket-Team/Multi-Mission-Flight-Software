#include "Psram.h"

PSRAM::PSRAM()
{
    ready = false;
    dumped = false;

    cursorStart = nullptr;
    cursorEnd = nullptr;
}

bool PSRAM::init()
{
    uint8_t size = external_psram_size;
    memBegin = cursorStart = reinterpret_cast<char *>(0x70000000);
    memEnd = cursorEnd = memBegin + (size * 1048576);

    if (size > 0)
    {
        ready = true;
    }

    return ready;
}
void PSRAM::println(const char *data, bool writeToTop = true)
{
    if (ready)
    {
        print(data, writeToTop);
        print("\n", writeToTop);
    }
}

// Write string to FRAM
void PSRAM::print(const char *data, bool writeToTop = true)
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
bool PSRAM::isReady()
{
    return ready;
}

int PSRAM::getFreeSpace()
{
    return cursorEnd - cursorStart;
}

char *PSRAM::read(char *data, int size)
{
    if (ready)
    {
        for (int i = 0; i < size; i++)
        {
            data[i] = *cursorStart;
            cursorStart++;
        }
    }
    return data;
}

char *PSRAM::read(char *data)
{
    if (ready)
    {
        char *mem = memBegin;
        while (mem != cursorStart)
        {
            *data = *mem;
            mem++;
            data++;
        }
    }
    return data;
}

char *PSRAM::readTo(char *data, char endChar)
{
    if (ready)
    {
        char *mem = memBegin;
        while (*mem != endChar && mem != cursorStart)
        {
            *data = *mem;
            mem++;
            data++;
        }
    }
    return data;
}

char *PSRAM::readFromBottom(char *data, int size)
{
    if (ready)
    {
        char *mem = memEnd;
        int i = 0;
        while (i < size && mem != cursorEnd) {
            *data = *mem;
            mem--;
            data++;
        }
    }

    return data;
}

char *PSRAM::readFromBottom(char *data)
{
    if (ready)
    {
        char *mem = memEnd;
        while (mem != cursorEnd)
        {
            *data = *mem;
            mem--;
            data++;
        }
    }

    return data;
}

bool PSRAM::seek(int offset)
{
    if (ready)
    {
        cursorStart += offset;
        return true;
    }
    return false;
}

bool PSRAM::seekFromBottom(int offset)
{
    if (ready)
    {
        cursorEnd -= offset;
        return true;
    }
    return false;
}