#ifndef ARR_PRINT_H
#define ARR_PRINT_H

#include <Print.h>
#include <Utils/CircBuffer.h>

class ArrPrint : public Print
{
public:
    ArrPrint(int len) : len(len)
    {
        buffer = new CircBuffer<char>(len);
        arr = new char[len + 1];
    }

    ~ArrPrint()
    {
        delete buffer;
    }

    size_t write(uint8_t b) override
    {
        buffer->push(b);
        return 1;
    }

    char *getArr()
    {
        for (int i = 0; i < buffer->getCount(); i++)
            arr[i] = (*buffer)[i];
        arr[buffer->getCount()] = '\0';
        return arr;
    }

private:
    CircBuffer<char> *buffer;
    char *arr;
    int len;
};

#endif