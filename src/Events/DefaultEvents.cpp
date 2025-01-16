#include "Event.h"
#include "../RecordData/Logger.h"

namespace mmfs
{
    struct GPS_FIX : public Event
    {
        bool hasFix;
    };
    struct LOG_DATA : public Event
    {
        Dest dest;
        LogType type;
        char *data;
    };
}