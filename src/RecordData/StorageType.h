#ifndef STORAGE_TYPE_H
#define STORAGE_TYPE_H

#include <cstdint>

namespace mmfs {

class StorageType
{
public:
    virtual ~StorageType() {};

    /**
     * Initialize the storage device.
     * @return true if initialization was successful, false otherwise.
     */
    virtual bool init() = 0;

    /**
     * Check if the storage device is ready for read/write operations.
     * @return true if the storage device is ready, false otherwise.
     */
    virtual bool isReady() const = 0;

    /**
     * Write data to the storage device.
     * @param data The data to be written.
     * @param size The size of the data to be written.
     */
    virtual void write(char *data, int size) = 0;

    /**
     * Read data from the storage device.
     * @param data The buffer to store the read data.
     * @param size The size of the data to be read.
     * @return The number of bytes read. If unsuccessful, returns -1.
     */
    virtual int read(char *data, int size) = 0;

    /**
     * Read data from the storage device until a specified end character is encountered.
     * @param data The buffer to store the read data.
     * @param endChar The character to stop reading at.
     * @return The number of bytes read. If unsuccessful, returns -1.
     */
    virtual int readTo(char *data, char endChar) = 0;

    /**
     * Seek to a specific offset in the storage device.
     * @param offset The offset to seek to. Relative to the beginning of the file.
     * @return true if the seek operation was successful, false otherwise.
     */
    virtual bool seek(uint64_t offset) = 0;
};

} // namespace mmfs

#endif // STORAGE_TYPE_H