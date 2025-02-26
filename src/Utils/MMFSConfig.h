#ifndef MMFS_CONFIG_H
#define MMFS_CONFIG_H

#include "Events/DefaultEventHandler.h"

namespace mmfs
{
    class State;
    class DataReporter;
    class MMFSConfig
    {
        friend class MMFSSystem;

    public:
        // Add state (and its sensors) to MMFS's knowledge.
        // No default.
        MMFSConfig &withState(State *state);
        // Set an update rate (in hz).
        // Default `10`.
        MMFSConfig &withUpdateRate(unsigned int updateRate);
        // Set an update Interval (in ms). 
        // Default `100`.
        MMFSConfig &withUpdateInterval(unsigned int updateInterval);
        // Set the duration that sensors will average over to correct for drift (in s)
        // Affected by update rate/update interval.
        // Default `2`.
        MMFSConfig &withSensorBiasCorrectionDataLength(unsigned int sensorBiasCorrectionDataLength);
        // Set the duration of the most recent data to ignore for drift correction averaging.(in s)
        // Affected by update rate/update interval.
        // Default `1`.
        MMFSConfig &withSensorBiasCorrectionDataIgnore(unsigned int sensorBiasCorrectionDataIgnore);
        // Determines if the sensors will continuously re-zero themselves while on the ground.
        // Default `false`.
        // ***DANGER** Must have working launch detection or data will not be accurate.
        MMFSConfig &withUsingSensorBiasCorrection(bool useBiasCorrection);
        // Set the named `BUZZER_PIN` for use with `BlinkBuzz`.
        // Default `-3`.
        MMFSConfig &withBuzzerPin(unsigned int buzzerPin);
        // Add a pin to `BlinkBuzz`. Without this, BB will refuse to toggle the pin.
        // No pins added by default.
        MMFSConfig &withBBPin(unsigned int bbPin);
        // Allow `BlinkBuzz` to use Async features.
        // Incurs moderate memory overhead based on `queueSize`, which is the number of state changes a pin can queue up at once.
        // Default `true`, `50`.
        MMFSConfig &withBBAsync(bool bbAsync, unsigned int queueSize = 50);
        // Add more `DataReporter` objects for Logger to record flight data from.
        // Passing in a State (via `withState()`) will also capture the state's sensors for logging, so adding them here is redundant.
        // No other `DataReporter`s added by default.
        MMFSConfig &withOtherDataReporters(DataReporter **others);
        // Remove the default event handler from the event manager.
        // Useful if you have your own that alters the behavior.
        MMFSConfig &withNoDefaultEventListener();
        // Change the formatting of the log prefix.
        // use $time and $logType to access the time and log type of the current log.
        // Default: `"$time - [$logType] "`
        MMFSConfig &withLogPrefixFormatting(const char *prefix);

        MMFSConfig();

    private:
        State *state = nullptr;
        int pins[50];
        bool bbAsync;
        unsigned int maxQueueSize = 50;
        DataReporter *reporters[50];
        bool useBiasCorrection;
        IEventListener *defaultEventListener;

        uint8_t numReporters = 0;
    };
}
#endif