#ifndef _DEFAULT_EVENTS_H
#define _DEFAULT_EVENTS_H
#include "Event.h"
#include "../RecordData/Logger.h"
namespace mmfs
{
    enum Dest;
    class GPSFix : public Event
    {
    public:
        GPSFix(EventID id, bool hasFix) : Event(id), hasFix(hasFix) {}
        bool hasFix;
    };
    class LogData : public Event
    {
    public:
        LogData(EventID id, Dest dest, LogType type, const char *data) : Event(id), dest(dest), type(type), data(data) {}
        Dest dest;
        LogType type;
        const char *data;
    };
}

#endif // _DEFAULT_EVENTS_H