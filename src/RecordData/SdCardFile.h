#ifndef SD_CARD_FILE_H
#define SD_CARD_FILE_H

#include "StorageType.h"
#include "SdFat.h"

class SdCardFile : public StorageType
{
public:
    char *fileName;

    SdCardFile();
    bool init();
    bool isReady();
    void print(const char *data);
    void println(const char *data);
    char *read(char *data, int size);
    char *readTo(char *data, char endChar);
    bool seek(int offset);
};


#endif //SD_CARD_FILE_H