#ifndef DEFAULT_EVENT_HANDLER_H
#define DEFAULT_EVENT_HANDLER_H

#include "DefaultEvents.h"

namespace mmfs
{
    class DefaultEventHandler : public IEventListener
    {
    public:
        void onEvent(const Event *event) override;

    protected:
        void handleGPSFix(const GPSFix *event);
        void handleLogData(const LogData *event);
        void handleInitEvent(const BoolEvent *event);

        void bb(bool OK); //blink buzz on/off based on "ok" or "not ok"
        void abb(bool OK); //blink buzz on/off based on "ok" or "not ok" asynchronously
    };
}

#endif