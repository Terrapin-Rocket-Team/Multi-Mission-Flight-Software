#ifndef STORAGE_TYPE_H
#define STORAGE_TYPE_H

class StorageType
{

public:
    virtual ~StorageType() {};
    virtual bool init() = 0;
    virtual bool isReady() = 0;
    virtual void print(const char *data) = 0;
    virtual void println(const char *data) = 0;
    virtual char *read(char *data, int size) = 0;
    virtual char *readTo(char *data, char endChar) = 0;
    virtual bool seek(int offset) = 0;
};

#endif STORAGE_TYPE_H