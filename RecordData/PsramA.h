#ifndef PSRAM_H
#define PSRAM_H

#include "StorageType.h"

class PSRAM: public StorageType
{

public:
    PSRAM();
    bool init();
    bool isReady();
    void print(const char *data, bool writeToTop = true);
    void println(const char *data, bool writeToTop = true);
    int getFreeSpace();
    char *read(char *data, int size);
    char *readTo(char *data, char endChar);
    bool seek(int offset);
};



#endif //PSRAM_H