#ifndef STORAGE_TYPE_H
#define STORAGE_TYPE_H

class StorageType
{

public:
    virtual ~StorageType() {};
    virtual bool init() = 0;
    virtual bool isReady() const = 0;
    virtual void write(char *data, int size) = 0;
    virtual int read(char *data, int size) = 0;
    virtual int readTo(char *data, char endChar) = 0;
    virtual bool seek(uint64_t offset) = 0;
};

#endif STORAGE_TYPE_H