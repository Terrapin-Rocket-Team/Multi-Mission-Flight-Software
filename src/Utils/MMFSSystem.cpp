#include "MMFSSystem.h"
#include "Error/ErrorHandler.h"
#include "BlinkBuzz/BlinkBuzz.h"
#include "State/State.h"

using namespace mmfs;
BlinkBuzz bb;
ErrorHandler errorHandler;
MMFSSystem::MMFSSystem(MMFSConfig *config) : config(config)
{
}
void MMFSSystem::init()
{
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

    DataReporter **reporters = new DataReporter *[config->numReporters + config->state->getNumMaxSensors()];

    reporters[0] = config->state;
    int i = 1;
    for (; i < config->state->getNumMaxSensors(); i++)
        reporters[i] = config->state->getSensors()[i - 1];
    int j = i;
    for (i = 0; i < config->numReporters; i++)
        reporters[j++] = config->reporters[i];

    bool log = getLogger().init(reporters, j, 30, 30);
    getEventManager().invoke(BoolEvent{"LOGGER_INIT"_i, log});

    delete[] reporters;

    // then State

    bool state = config->state->init(config->useBiasCorrection);
    getEventManager().invoke(BoolEvent{"STATE_INIT"_i, state});
    ready = true;

    getLogger().writeCsvHeader();
}
bool MMFSSystem::update(double ms)
{
    if (!ready)
    {
        getLogger().recordLogData(WARNING_, "Attempted to update MMFSSystem before it was initialized. Initializing it now...");
        init();
    }
    // loop based on time and interval and update bb.
    bb.update();
    if (ms == -1)
        ms = millis();
    if (ms - lastUdpate > UPDATE_INTERVAL)
    {
        lastUdpate = ms;
        if (config->state)
            config->state->updateState();
        else
            getLogger().recordLogData(WARNING_, "MMFS Attempted to udpate State without a reference to it! (use MMFSConfig.withStat(&stateVar))");
        return true;
    }
    return false;
}