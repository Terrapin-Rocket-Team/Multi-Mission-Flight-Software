#ifndef PSRAM_H
#define PSRAM_H

#include "StorageType.h"
#include <cstdint>

extern "C" uint8_t external_psram_size;
class PSRAM: public StorageType
{

private:
    bool ready;
    bool dumped;
    char *cursorStart;//for flight data
    char *cursorEnd;//for log data
    const char *memBegin;//start of memory
    const char *memEnd;//end of memory

public:
    PSRAM();
    bool init();
    bool isReady();
    void print(const char *data, bool writeToTop = true);
    void println(const char *data, bool writeToTop = true);
    int getFreeSpace();
    char *read(char *data, int size);
    char *read(char *data);
    char *readTo(char *data, char endChar);
    char *readFromBottom(char *data, int size);
    char *readFromBottom(char *data);
    bool seek(int offset);
    bool seekFromBottom(int offset);
};





#endif //PSRAM_H