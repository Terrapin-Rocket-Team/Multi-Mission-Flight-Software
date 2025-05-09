#ifndef _DEFAULT_EVENTS_H
#define _DEFAULT_EVENTS_H
#include "Event.h"
#include "../RecordData/Logging/Logger.h"
namespace mmfs
{
    class GPS;
    class GPSFix : public Event
    {
    public:
        GPSFix(EventID id, GPS *gps, bool firstFix) : Event(id), gps(gps), firstFix(firstFix) {}
        GPS *gps;
        bool firstFix;
    };
    class LogData : public Event
    {
    public:
        LogData(EventID id, Dest dest, LogType type, const char *data) : Event(id), dest(dest), type(type), data(data) {}
        Dest dest;
        LogType type;
        const char *data;
    };

    class BoolEvent : public Event
    {
    public:
        BoolEvent(EventID id, bool value) : Event(id), value(value) {}
        bool value;
    };
}

#endif // _DEFAULT_EVENTS_H