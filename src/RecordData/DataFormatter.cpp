#include "DataFormatter.h"

#include "../State/State.h"
#include "DataReporter.h"

using namespace mmfs;

uintptr_t DataFormatter::packData(uintptr_t dest, State *state)
{
    // pack state data
    memcpy((void *)dest, state->getPackedData(), state->getPackedDataSize());
    dest += state->getPackedDataSize();

    // pack sensor data
    for (int i = 0; i < state->getNumMaxSensors(); i++)
    {
        if (!state->sensorOK(state->getSensors()[i]))
            continue;
        memcpy((void *)dest, state->getSensors()[i]->getPackedData(), state->getSensors()[i]->getPackedDataSize());
        dest += state->getSensors()[i]->getPackedDataSize();
    }

    return dest;
}

void DataFormatter::toCSVRow(char *dest, int destLen, State *state, void *data)
{
    // state first
    uintptr_t cursor = toCSVSection(dest, destLen, data == nullptr ? state->getPackedData() : (void *)data, state);

    // sensors next
    for (int i = 0; i < state->getNumMaxSensors(); i++)
    {
        if (!state->sensorOK(state->getSensors()[i]))
            continue;
        cursor = toCSVSection((char *)cursor, destLen, data == nullptr ? state->getSensors()[i]->getPackedData() : (void *)data, state->getSensors()[i]);
    }
    cursor = '\0';
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
            int len = strlen(labels[j]) + 1;
            snprintf(dest + offset, destLen, "%s,", labels[j]);
            offset += len;
            destLen -= len;
        }
    }
}

uintptr_t DataFormatter::toCSVSection(char *dest, int &destLen, void *data, DataReporter *obj)
{
    int offset = 0;
    for (int i = 0; i < obj->getNumPackedDataPoints() && destLen > 0; i++)
    {
        const void *dataPtr = (uint8_t *)data + offset;
        int size = obj->PackedTypeToSize(obj->getPackedOrder()[i]);

        switch (obj->getPackedOrder()[i])
        {
        case FLOAT:
            snprintf(dest, destLen, "%.3f,", *(float *)dataPtr);
            break;
        case DOUBLE:
            snprintf(dest, destLen, "%.3f,", *(double *)dataPtr);
            break;
        case BYTE:
            snprintf(dest, destLen, "%d,", *(uint8_t *)dataPtr);
            break;
        case SHORT:
            snprintf(dest, destLen, "%d,", *(int16_t *)dataPtr);
            break;
        case INT:
            snprintf(dest, destLen, "%ld,", *(int32_t *)dataPtr);
            break;
        case LONG:
            snprintf(dest, destLen, "%lld,", *(int64_t *)dataPtr);
            break;
        case STRING_10:
        case STRING_20:
        case STRING_50:
            snprintf(dest, destLen, "%s,", (char *)dataPtr);
            break;
        default:
            break;
        }
        offset += size;
        destLen -= size;
    }
    return (uintptr_t)dest + offset;
}
