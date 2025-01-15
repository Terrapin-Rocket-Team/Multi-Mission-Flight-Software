#include "DataFormatter.h"
#include "DataReporter.h"

using namespace mmfs;

uint8_t *DataFormatter::packData(uint8_t *dest, DataReporter **drs, int numDrs)
{
    int offset = 0;
    for (int i = 0; i < numDrs; i++)
    {
        memcpy((uint8_t *)dest + offset, drs[i]->getPackedData(), drs[i]->getPackedDataSize());
        offset += drs[i]->getPackedDataSize();
    }

    return dest + offset;
}

int DataFormatter::getPackedLen(DataReporter **drs, int numDrs)
{
    int len = 0;
    for (int i = 0; i < numDrs; i++)
    {
        len += drs[i]->getPackedDataSize();
    }
    return len;
}

void DataFormatter::getCSVHeader(char *dest, int destLen, DataReporter **drs, int numDrs)
{
    int offset = 0;
    // sensors next
    for (int i = 0; i < numDrs; i++)
    {
        auto curDataPt = drs[i]->getPackedInfo();
        for (int j = 0; j < drs[i]->getNumColumns() && destLen > 0; j++)
        {

            int len = snprintf(dest + offset, destLen, "%s - %s,", drs[i]->getName(), curDataPt->label);
            offset += len;
            destLen -= len;
            curDataPt = curDataPt->next;
        }
    }
    dest[offset - 1] = '\0';
}

void DataFormatter::toCSVRow(char *dest, int destLen, DataReporter **drs, int numDrs, void *data)
{
    int dataOffset = 0;

    uintptr_t cursor = 0;
    for (int i = 0; i < numDrs; i++)
    {
        if (data == nullptr)
            dataOffset = 0;
        cursor = toCSVSection((char *)cursor, destLen, data == nullptr ? drs[i]->getPackedData() : (void *)data, dataOffset, drs[i]);
    }
    ((char *)cursor)[-1] = '\0';
}

uintptr_t DataFormatter::toCSVSection(char *dest, int &destLen, void *data, int &dataOffset, DataReporter *obj)
{
    int strSize = 0;
    auto curDataPt = obj->getPackedInfo();
    for (int i = 0; i < obj->getNumColumns() && destLen > 0; i++)
    {
        const void *dataPtr = (uint8_t *)data + dataOffset;
        dataOffset += curDataPt->size;
        int printedSize = 0;
        switch (curDataPt->type)
        {
        case FLOAT:
            printedSize = snprintf(dest + strSize, destLen, "%.3f,", *(float *)dataPtr);
            break;
        case DOUBLE:
            printedSize = snprintf(dest + strSize, destLen, "%.7f,", *(double *)dataPtr);
            break;
        case INT:
            printedSize = snprintf(dest + strSize, destLen, "%d,", *(int *)dataPtr);
            break;
        case LONG:
            printedSize = snprintf(dest + strSize, destLen, "%ld,", *(long int *)dataPtr);
            break;
        case STRING:
            printedSize = snprintf(dest + strSize, destLen, "%s,", (char *)dataPtr);
            break;
        case BOOL:
            printedSize = snprintf(dest + strSize, destLen, "%s,", *((bool *)dataPtr) ? "True" : "False");
        default:
            break;
        }
        destLen -= printedSize;
        strSize += printedSize;
    }
    return (uintptr_t)dest + strSize;
}
