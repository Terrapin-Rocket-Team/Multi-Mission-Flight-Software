#include "DefaultEventHandler.h"
#include "Sensors/GPS/GPS.h"
#include "BlinkBuzz/BlinkBuzz.h"
#include <Arduino.h>
using namespace mmfs;

void DefaultEventHandler::onEvent(const Event *e)
{
    switch (e->ID)
    {
    case "GPS_FIX"_i:
        handleGPSFix(static_cast<const GPSFix *>(e));
        break;
    case "LOG_DATA"_i:
        handleLogData(static_cast<const LogData *>(e));
        break;
    case "LOGGER_INIT"_i:
    case "STATE_INIT"_i:
        handleInitEvent(static_cast<const BoolEvent *>(e));
        break;
    default:
        break;
    }
}

void DefaultEventHandler::handleGPSFix(const GPSFix *e)
{

    const GPSFix *fix = static_cast<const GPSFix *>(e);
    if (fix->gps->getHasFix())
    {
        if (fix->firstFix)
            getLogger().modifyFileDates(fix->gps);
            
        getLogger().recordLogData(INFO_, 50, "The %s acquired a satellite fix.", fix->gps->getName());
    }
    else
        getLogger().recordLogData(WARNING_, 50, "The %s lost its fix.", fix->gps->getName());
}

void DefaultEventHandler::handleLogData(const LogData *e)
{
}

void DefaultEventHandler::handleInitEvent(const BoolEvent *e)
{    
    bb(e->value); // blink buzz on/off based on "ok" or "not ok"
}

void DefaultEventHandler::bb(bool OK)
{
    if (OK)
        ::bb.onoff(BUZZER, 1000, 1);
    else
        ::bb.onoff(BUZZER, 200, 3);
}

void DefaultEventHandler::abb(bool OK)
{
    if (OK)
        ::bb.aonoff(BUZZER, 1000, 1);
    else
        ::bb.aonoff(BUZZER, 200, 3);
}