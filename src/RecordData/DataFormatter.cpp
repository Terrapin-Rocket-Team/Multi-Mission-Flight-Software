#include "DataFormatter.h"
#include "DataReporter.h"

using namespace mmfs;

void DataFormatter::getCSVHeader(char *dest, int destLen, DataReporter **drs, int numDrs)
{
    int offset = 0;
    for (int i = 0; i < numDrs; i++)
    {
        auto curDataPt = drs[i]->getDataPoints();
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
    char *cursor = dest;
    for (int i = 0; i < numDrs; i++)
        cursor = toCSVSection(cursor, destLen, drs[i]);
    cursor[-1] = '\0'; // Remove the last comma
}

char *DataFormatter::toCSVSection(char *dest, int &destLen, DataReporter *obj)
{
    int strSize = 0;
    auto curDataPt = obj->getDataPoints();
    for (int i = 0; i < obj->getNumColumns() && destLen > 0; i++)
    {
        const void *dataPtr = curDataPt->data;
        int printedSize = 0;
        switch (curDataPt->type)
        {
        case FLOAT:
            printedSize = snprintf(dest + strSize, destLen, "%.3f,", *(float *)dataPtr);
            break;
        case DOUBLE:
            printedSize = snprintf(dest + strSize, destLen, "%.3f,", *(double *)dataPtr);
            break;
        case DOUBLE_HP:
            printedSize = snprintf(dest + strSize, destLen, "%.7f,", *(double *)dataPtr);
            break;
        case INT:
            printedSize = snprintf(dest + strSize, destLen, "%d,", *(int *)dataPtr);
            break;
        case BYTE:
            printedSize = snprintf(dest + strSize, destLen, "%hhd,", *(int8_t *)dataPtr);
            break;
        case SHORT:
            printedSize = snprintf(dest + strSize, destLen, "%hd,", *(int16_t *)dataPtr);
            break;
        case LONG:
            printedSize = snprintf(dest + strSize, destLen, "%ld,", *(long *)dataPtr);
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
        curDataPt = curDataPt->next;
    }
    return dest + strSize;
}
