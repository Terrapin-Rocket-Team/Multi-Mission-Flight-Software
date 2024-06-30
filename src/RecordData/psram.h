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
    char *memBegin;//start of memory
    char *memEnd;//end of memory

public:
    PSRAM();
    bool init() override;
    bool isReady() const override;
    void write(char *data, int size) override;
    void write(char *data, int size, bool writeToTop);
    void print(const char *data) { print(data, true); };
    void print(const char *data, bool writeToTop);
    void println(const char *data) { println(data, true); };
    void println(const char *data, bool writeToTop);
    int getFreeSpace() const;
    int getDataSpace() const;
    int getLogSpace() const;
    int read(char *data, int size) override;
    int read(char *data);
    int readTo(char *data, char endChar) override;
    int readFromBottom(char *data, int size);
    int readFromBottom(char *data);
    bool seek(uint64_t offset) override;
    bool seekFromBottom(uint64_t offset);
};





#endif //PSRAM_H