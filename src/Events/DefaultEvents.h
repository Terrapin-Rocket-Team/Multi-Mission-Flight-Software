#ifndef _DEFAULT_EVENTS_H
#define _DEFAULT_EVENTS_H
#include "Event.h"
#include "../RecordData/Logger.h"
namespace mmfs
{
    enum Dest;
    class GPS_FIX : public Event
    {
    public:
        GPS_FIX(EventID id, bool hasFix) : Event(id), hasFix(hasFix) {}
        bool hasFix;
    };
    class LOG_DATA : public Event
    {
    public:
        LOG_DATA(EventID id, Dest dest, LogType type, char *data) : Event(id), dest(dest), type(type), data(data) {}
        Dest dest;
        LogType type;
        char *data;
    };
}

#endif // _DEFAULT_EVENTS_H