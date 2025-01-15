#include "DataReporter.h"

namespace mmfs
{

    int DataReporter::numReporters = 0;

    DataReporter::DataReporter(const char *name) : packedData(nullptr), packedDataSize(0)
    {
        numReporters++;
        if (name == nullptr)
        {
            this->name = new char[13];
            snprintf(this->name, 13, "Reporter%d", numReporters);
        }
        else
        {
            int len = strlen(name);
            this->name = new char[len + 1];
            snprintf(this->name, len + 1, "%s", name);
        }
    }

    DataReporter::~DataReporter()
    {
        delete[] packedData;
        delete[] name;
    }

    const char *DataReporter::getName() const
    {
        return name;
    }

    void DataReporter::setName(const char *n)
    {
        delete[] name;
        int len = strlen(n);
        name = new char[len + 1];
        snprintf(name, len + 1, "%s", n);
    }

    uint8_t *DataReporter::getPackedData()
    {
        return packedData;
    }

    int DataReporter::getPackedDataSize()
    {
        return packedDataSize;
    }

    int DataReporter::getNumColumns()
    {
        return fieldCount;
    }

    PackedInfo *DataReporter::getPackedInfo()
    {
        return first;
    }

    void DataReporter::packData()
    {
        if (packedData == nullptr)
            initializeDataReporting();

        auto current = first;
        int idx = 0;
        while (current != nullptr)
        {
            switch (current->type)
            {
            case INT:
                ((int *)packedData)[idx] = *((int *)current->data);
                break;
            case FLOAT:
                ((float *)packedData)[idx] = *((float *)current->data);
                break;
            case DOUBLE:
                ((double *)packedData)[idx] = *((double *)current->data);
                break;
            case STRING:
                snprintf((char *)&packedData[idx], current->size, "%s", (char *)current->data);
                break;
            case BOOL:
                ((bool *)packedData)[idx] = *((bool *)current->data);
                break;
            case LONG:
                ((long *)packedData)[idx] = *((long *)current->data);
                break;
            default:
                break;
            }
            idx += current->size;
            current = current->next;
        }
    }

    void DataReporter::initializeDataReporting()
    {
        delete[] packedData;
        packedData = new uint8_t[packedDataSize];
    }

    uint8_t DataReporter::findSizeOfType(PackedType t, void *str)
    {
        switch (t)
        {
        case INT:
        case BOOL:
            return sizeof(int);
        case FLOAT:
            return sizeof(float);
        case DOUBLE:
            return sizeof(double);
        case LONG:
            return sizeof(long);
        case STRING:
        {
            int size = (strlen((char *)str) + 1);
            size += size % 4; // Make sure the size is a multiple of 4 for alignment
            return size;
        }
        default:
            getLogger().recordLogData(INFO_, "Error: unknown type of DataReporter data");
            return 0;
        }
    }

} // namespace mmfs
