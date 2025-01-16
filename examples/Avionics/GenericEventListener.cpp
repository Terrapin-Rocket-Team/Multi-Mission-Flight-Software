#include "Events/Event.h"
#include "BlinkBuzz/BlinkBuzz.h"
#include "AviConstants.h"

using namespace mmfs;

void gpsFix(bool b);

class GenericEventListener : public IEventListener
{
public:
    GenericEventListener() : IEventListener() {}
    virtual ~GenericEventListener() {}

    virtual void onEvent(strToInt eventID, const void *data) override
    {
        switch (eventID)
        {
        case "GPS_FIX"_i:
            gpsFix(*(bool *)data);
            break;
        default:
            break;
        }
    }
};

void gpsFix(bool b)
{
    if (!b)
        bb.aonoff(GPS_LED, {200, 1}, true);
    else
    {
        bb.clearQueue(GPS_LED);
        bb.on(GPS_LED);
    }
}