#include "DataFormatter.h"

#include "../State/State.h"
#include "DataReporter.h"

using namespace mmfs;

uint8_t *DataFormatter::packData(uint8_t *dest, State *state)
{
    int offset = 0;
    // pack state data
    memcpy((void *)dest, state->getPackedData(), state->getPackedDataSize());
    offset += state->getPackedDataSize();

    // pack sensor data
    for (int i = 0; i < state->getNumMaxSensors(); i++)
    {
        if (!state->sensorOK(state->getSensors()[i]))
            continue;
        memcpy((uint8_t *)dest + offset, state->getSensors()[i]->getPackedData(), state->getSensors()[i]->getPackedDataSize());
        offset += state->getSensors()[i]->getPackedDataSize();
    }

    return dest + offset;
}

int DataFormatter::getPackedLen(State *state)
{
    int len = state->getPackedDataSize();
    for (int i = 0; i < state->getNumMaxSensors(); i++)
    {
        if (!state->sensorOK(state->getSensors()[i]))
            continue;
        len += state->getSensors()[i]->getPackedDataSize();
    }
    return len;
}

void DataFormatter::getCSVHeader(char *dest, int destLen, State *state)
{
    int offset = 0;
    // state first
    const char **labels = state->getPackedDataLabels();
    for (int i = 0; i < state->getNumPackedDataPoints() && destLen > 0; i++)
    {
        int len = strlen(labels[i]) + 1;
        snprintf(dest + offset, destLen, "%s,", labels[i]);
        offset += len;
        destLen -= len;
    }

    // sensors next
    for (int i = 0; i < state->getNumMaxSensors(); i++)
    {
        if (!state->sensorOK(state->getSensors()[i]))
            continue;
        labels = state->getSensors()[i]->getPackedDataLabels();
        for (int j = 0; j < state->getSensors()[i]->getNumPackedDataPoints() && destLen > 0; j++)
        {

            int len = snprintf(dest + offset, destLen, "%s,", labels[j]);
            offset += len;
            destLen -= len;
        }
    }
    dest[offset - 1] = '\0';
}

void DataFormatter::toCSVRow(char *dest, int destLen, State *state, void *data)
{
    int dataOffset = 0;
    // state first
    uintptr_t cursor = toCSVSection(dest, destLen, data == nullptr ? state->getPackedData() : (void *)data, dataOffset, state);

    // sensors next
    for (int i = 0; i < state->getNumMaxSensors(); i++)
    {
        if (!state->sensorOK(state->getSensors()[i]))
            continue;
        if (data == nullptr)
            dataOffset = 0;
        cursor = toCSVSection((char *)cursor, destLen, data == nullptr ? state->getSensors()[i]->getPackedData() : (void *)data, dataOffset, state->getSensors()[i]);
    }
    ((char *)cursor)[-1] = '\0';
}

uintptr_t DataFormatter::toCSVSection(char *dest, int &destLen, void *data, int &dataOffset, DataReporter *obj)
{
    int strSize = 0;
    for (int i = 0; i < obj->getNumPackedDataPoints() && destLen > 0; i++)
    {
        const void *dataPtr = (uint8_t *)data + dataOffset;
        dataOffset += obj->PackedTypeToSize(obj->getPackedOrder()[i]);
        int printedSize = 0;
        switch (obj->getPackedOrder()[i])
        {
        case FLOAT:
            printedSize = snprintf(dest + strSize, destLen, "%.3f,", *(float *)dataPtr);
            break;
        case DOUBLE:
            printedSize = snprintf(dest + strSize, destLen, "%.3f,", *(double *)dataPtr);
            break;
        case BYTE:
            printedSize = snprintf(dest + strSize, destLen, "%hhd,", *(uint8_t *)dataPtr);
            break;
        case SHORT:
            printedSize = snprintf(dest + strSize, destLen, "%hd,", *(int16_t *)dataPtr);
            break;
        case INT:
            printedSize = snprintf(dest + strSize, destLen, "%d,", *(int32_t *)dataPtr);
            break;
        case LONG:
            printedSize = snprintf(dest + strSize, destLen, "%ld,", *(int64_t *)dataPtr);
            break;
        case STRING_10:
        case STRING_20:
        case STRING_50:
            printedSize = snprintf(dest + strSize, destLen, "%s,", (char *)dataPtr);
            break;
        default:
            break;
        }
        destLen -= printedSize;
        strSize += printedSize;
    }
    return (uintptr_t)dest + strSize;
}
