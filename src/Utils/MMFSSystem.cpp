#include "MMFSSystem.h"
#include "BlinkBuzz/BlinkBuzz.h"
#include "State/State.h"
#include "Wire.h"
#include "RetrieveData/SerialHandler.h"

using namespace mmfs;
BlinkBuzz bb;
MMFSSystem::MMFSSystem(MMFSConfig *config) : config(config)
{
}
void MMFSSystem::init()
{
    getLogger().recordCrashReport();
    getLogger().recordLogData(INFO_, "Initializing MMFS.");
    Wire.begin();
    // BlinkBuzz first
    int pins = 0;
    for (int i = 0; i < 50; i++)
    {
        if (config->pins[i] == -1)
            break;
        pins++;
    }
    bb.init(config->pins, pins, config->bbAsync, config->maxQueueSize);

    // then Logger

    DataReporter **reporters = new DataReporter *[config->numReporters + config->state->getNumMaxSensors() + 1];

    reporters[0] = config->state;
    int i = 1;
    for (; i < config->state->getNumMaxSensors() + 1; i++)
        reporters[i] = config->state->getSensors()[i - 1];
    int j = i;
    for (i = 0; i < config->numReporters; i++)
        reporters[j++] = config->reporters[i];

    bool log = getLogger().init(reporters, j);
    getEventManager().invoke(BoolEvent{"LOGGER_INIT"_i, log});

    delay(10);
    // then State
    bool state = config->state->init(config->useBiasCorrection);
    getEventManager().invoke(BoolEvent{"STATE_INIT"_i, state});
    ready = true;

    getLogger().writeCsvHeader();
    getLogger().recordLogData(INFO_, "MMFS initialized.");
}
bool MMFSSystem::update(double ms)
{
    if (!ready)
    {
        getLogger().recordLogData(WARNING_, "Attempted to update MMFSSystem before it was initialized. Initializing it now...");
        init();
    }
    getSerialHandler().handle();
    // loop based on time and interval and update bb.
    bb.update();
    if (ms == -1)
        ms = millis();
    if (ms - lastUpdate > UPDATE_INTERVAL)
    {
        lastUpdate = ms;
        if (config->state)
            config->state->updateState();
        else
            getLogger().recordLogData(WARNING_, "MMFS Attempted to update State without a reference to it! (use MMFSConfig.withState(&stateVar))");
        getLogger().recordFlightData();
        return true;
    }
    return false;
}