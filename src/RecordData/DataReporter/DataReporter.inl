#ifndef DATA_REPORTER_INL
#define DATA_REPORTER_INL

namespace mmfs
{

    template <typename T>
    void DataReporter::insertColumn(uint8_t place, DataType t, T *variable, const char *label)
    {
        if (getLogger().isReady())
            getLogger().recordLogData(ERROR_, "Logger already initialized. Cannot add more columns.");

        auto packedInfo = new DataPoint();
        packedInfo->type = t;
        packedInfo->label = label;
        packedInfo->data = variable;

        if (first == nullptr)
        {
            first = packedInfo;
            last = packedInfo;
        }
        else if (place == -1 || place >= numColumns)
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
        numColumns++;
    }

    template <typename T>
    void DataReporter::addColumn(DataType t, T *variable, const char *label)
    {
        insertColumn(-1, t, variable, label);
    }

} // namespace mmfs

#endif // DATA_REPORTER_INL
