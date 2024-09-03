#ifndef PSRAM_H
#define PSRAM_H

#include "StorageType.h"
#include <cstdint>

namespace mmfs {

extern "C" uint8_t external_psram_size;
class PSRAM: public StorageType
{

private:
    bool ready;
    bool dumped;
    char *cursorStart; // for flight data
    char *cursorEnd; // for log data
    char *memBegin; // start of memory
    char *memEnd; // end of memory

public:
    /**
     * Constructor for PSRAM.
     */
    PSRAM();

    /**
     * Initialize the PSRAM.
     * @return true if initialization was successful, false otherwise.
     */
    bool init() override;

    /**
     * Check if the PSRAM is ready for read/write operations.
     * @return true if the PSRAM is ready, false otherwise.
     */
    bool isReady() const override;

    /**
     * Write data to the PSRAM.
     * @param data The data to be written.
     * @param size The size of the data to be written.
     */
    void write(char *data, int size) override;

    /**
     * Write data to the PSRAM with an option to write to the top.
     * @param data The data to be written.
     * @param size The size of the data to be written.
     * @param writeToTop If true, write to the top of the memory. If false, write to the bottom in reverse.
     */
    void write(char *data, int size, bool writeToTop);

    /**
     * Print a string to the PSRAM.
     * @param data The string to be printed. Takes a pointer to a null-terminated character array.
     */
    void print(const char *data) { print(data, true); };

    /**
     * Print a string to the PSRAM with an option to write to the top.
     * @param data The string to be printed. Takes a pointer to a null-terminated character array.
     * @param writeToTop If true, write to the top of the memory. If false, write to the bottom in reverse.
     */
    void print(const char *data, bool writeToTop);

    /**
     * Print a string to the PSRAM followed by a newline.
     * @param data The string to be printed. Takes a pointer to a null-terminated character array.
     */
    void println(const char *data) { println(data, true); };

    /**
     * Print a string to the PSRAM followed by a newline with an option to write to the top.
     * @param data The string to be printed.
     * @param writeToTop If true, write to the top of the memory. If false, write to the bottom in reverse.
     */
    void println(const char *data, bool writeToTop);

    /**
     * Get the amount of free space in the PSRAM.
     * @return The amount of free space in bytes.
     */
    int getFreeSpace() const;

    /**
     * Get the amount of data space used in the PSRAM.
     * @return The amount of data space used in bytes.
     */
    int getDataSpace() const;

    /**
     * Get the amount of log space used in the PSRAM. Checks from the bottom of the memory.
     * @return The amount of log space used in bytes.
     */
    int getLogSpace() const;

    /**
     * Read data from the PSRAM.
     * @param data The buffer to store the read data.
     * @param size The size of the data to be read, in bytes.
     * @return The number of bytes read. If unsuccessful, returns -1.
     */
    int read(char *data, int size) override;

    /**
     * Read data from the PSRAM. Reads all the filled in data.
     * @param data The buffer to store the read data.
     * @return The number of bytes read. If unsuccessful, returns -1.
     */
    int read(char *data);

    /**
     * Read data from the PSRAM until a specified end character is encountered.
     * @param data The buffer to store the read data.
     * @param endChar The character to stop reading at.
     * @return The number of bytes read. If unsuccessful, returns -1.
     */
    int readTo(char *data, char endChar) override;

    /**
     * Read data from the bottom of the PSRAM.
     * @param data The buffer to store the read data.
     * @param size The size of the data to be read.
     * @return The number of bytes read. If unsuccessful, returns -1.
     */
    int readFromBottom(char *data, int size);

    /**
     * Read all the filled in data from the bottom of the PSRAM.
     * @param data The buffer to store the read data.
     * @return The number of bytes read. If unsuccessful, returns -1.
     */
    int readFromBottom(char *data);

    /**
     * Seek to a specific offset in the PSRAM.
     * @param offset The offset to seek to, relative to the beginning of the memory.
     * @return true if the seek operation was successful, false otherwise.
     */
    bool seek(uint64_t offset) override;

    /**
     * Seek to a specific offset from the bottom in the PSRAM.
     * @param offset The offset to seek to, relative to the bottom of the memory.
     * @return true if the seek operation was successful, false otherwise.
     */
    bool seekFromBottom(uint64_t offset);
};




} // namespace mmfs

#endif //PSRAM_H