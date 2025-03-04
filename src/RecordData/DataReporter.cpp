#include "DataReporter.h"

namespace mmfs
{

    int DataReporter::numReporters = 0;

    DataReporter::DataReporter(const char *name)
    {
        numReporters++;
        if (name == nullptr)
        {
            this->name = new char[13];
            snprintf(this->name, 13, "Reporter #%d", numReporters);
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
        auto current = first;
        while (current != nullptr)
        {
            auto next = current->next;
            delete current;
            current = next;
        }
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

    int DataReporter::getNumColumns()
    {
        return numColumns;
    }

    DataPoint *DataReporter::getDataPoints()
    {
        return first;
    }

    void DataReporter::removeColumn(const char *label)
    {
        if (getLogger().isReady())
            getLogger().recordLogData(ERROR_, "Logger already initalized. Cannot remove any columns.");
        if (first == nullptr)
            return;
        if (strcmp(first->label, label) == 0)
        {
            auto toDel = first;
            first = first->next;
            if (first == nullptr)
                last = nullptr;
            delete toDel;
            numColumns--;
            return;
        }

        auto t = first;
        while (t->next != nullptr)
        {
            if (strcmp(t->next->label, label) == 0)
            {
                auto toDel = t->next;
                t->next = t->next->next;
                if (t->next == nullptr)
                    last = t;
                delete toDel;
                numColumns--;
                return;
            }
            t = t->next;
        }
    }

} // namespace mmfs
