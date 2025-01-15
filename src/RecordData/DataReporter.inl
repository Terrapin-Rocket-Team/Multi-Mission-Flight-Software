#ifndef DATA_REPORTER_INL
#define DATA_REPORTER_INL

namespace mmfs
{

    template <typename T>
    void DataReporter::insertColumn(uint8_t place, PackedType t, T *variable, const char *label)
    {
        if (getLogger().isReady())
            getLogger().recordLogData(ERROR_, "Logger already initialized. Cannot add more columns.");

        auto packedInfo = new PackedInfo();
        packedInfo->type = t;
        packedInfo->label = label;
        packedInfo->data = variable;
        packedInfo->size = findSizeOfType(t, variable);
        packedDataSize += packedInfo->size;

        if (first == nullptr)
        {
            first = packedInfo;
            last = packedInfo;
        }
        else if (place == -1 || place >= fieldCount)
        { // Append
            last->next = packedInfo;
            last = packedInfo;
        }
        else
        { // Insert at specific position
            auto current = first;
            int idx = 0;

            while (current->next != nullptr && idx < place - 1)
            {
                current = current->next;
                idx++;
            }

            packedInfo->next = current->next;
            current->next = packedInfo;

            if (packedInfo->next == nullptr)
            {
                last = packedInfo;
            }
        }
        fieldCount++;
    }

    template <typename T>
    void DataReporter::addColumn(PackedType t, T *variable, const char *label)
    {
        insertColumn(-1, t, variable, label);
    }

} // namespace mmfs

#endif // DATA_REPORTER_INL
