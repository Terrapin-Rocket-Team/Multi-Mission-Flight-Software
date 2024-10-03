#ifndef STORAGE_TYPE_H
#define STORAGE_TYPE_H

#include <cstdint>

namespace mmfs {

class StorageFile
{
public:
    virtual ~StorageFile() {};

    virtual void write(char *data, int size) = 0;

    virtual int read(char *data, int size) = 0;

    // virtual int readTo(char *data, char endChar) = 0;

    // virtual bool seek(uint64_t offset) = 0;
};

} // namespace mmfs

#endif // STORAGE_TYPE_H