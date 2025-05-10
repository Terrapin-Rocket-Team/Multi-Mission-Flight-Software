# MMFSSystem

The **MMFSSystem** object is designed to make interacting with the **MMFS** library as simple as possible. You begin by defining an **MMFSConfig** object, configuring all of the parameters your flight code needs, and then passing that configuration object into an **MMFSSystem** object. Afterward, you only need to call the `init()` and `update()` functions. **MMFSSystem** will take care of everything else.

## MMFSConfig

The **MMFSConfig** object uses a builder-like pattern, making it easy to set up **MMFS**. The only configuration option that you **must** use in order to run **MMFSSystem** is to pass a derived **State** object with proper launch detection.

### Code Example: `withState(...)`

```cpp
#include <MMFS.h>
#include <MyCustomState.h> // A user-defined State class that detects launch


MyCustomState myState;

MMFSConfig config = MMFSConfig()
                    .withState(&myState); // REQUIRED
```

Where `myState` is your derived **State** object that handles launch detection.

### MMFSConfig Options

Below is a refined list of the configuration methods available in **MMFSConfig**. Each returns a reference to the same **MMFSConfig** instance, allowing you to chain calls together:

- **`#!cpp withState(State *state)`**  
  Adds a derived **State** (and its associated sensors) to **MMFS**. This is required for **MMFSSystem** to function properly.

- **`#!cpp withUpdateRate(unsigned int updateRate)`**  
  Sets an update rate in Hertz. (Default is `10`.) Mutually exclusive with update interval.

- **`#!cpp withUpdateInterval(unsigned int updateInterval)`**  
  Sets the update interval in milliseconds. (Default is `100`.) Mutually exclusive with update rate.

- **`#!cpp withSensorBiasCorrectionDataLength(unsigned int sensorBiasCorrectionDataLength)`**  
  Specifies the duration (in seconds) over which sensors will average data to correct for drift. This duration is affected by the update rate/interval. (Default is `2`.)

- **`#!cpp withSensorBiasCorrectionDataIgnore(unsigned int sensorBiasCorrectionDataIgnore)`**  
  Specifies the duration (in seconds) of the most recent data to **ignore** when performing drift correction. This duration is also affected by the update rate/interval. (Default is `1`.)

- **`#!cpp withUsingSensorBiasCorrection(bool useBiasCorrection)`**  
  Determines whether sensors will continuously re-zero themselves while on the ground. (Default is `false`.)  
  **Warning**: This function requires working launch detection or data may not be accurate.

- **`#!cpp withBuzzerPin(unsigned int buzzerPin)`**  
  Sets the named `BUZZER` for use with `BlinkBuzz`. (No Default.)

- **`#!cpp withBBPin(unsigned int bbPin)`**  
  Adds a pin to `BlinkBuzz`. By default, no pins are added.

- **`#!cpp withBBAsync(bool bbAsync, unsigned int queueSize = 50)`**  
  Allows `BlinkBuzz` to use asynchronous features. This incurs moderate memory overhead based on the queue size (the number of state changes a pin can queue). (Default is `true` and `50`.)

- ~~**`#!cpp withReducedPreFlightDataRate(bool useReducedRate, unsigned int secondsBetweenRecords)`**  
  Enables or disables reduced data rates before flight. (Default is `true`, `30` seconds.)  
  **Warning**: This requires working launch detection or all data will remain at the reduced rate.~~

- **`#!cpp withOtherDataReporters(DataReporter **others)`**  
  Adds additional **DataReporter** objects for flight data logging. Passing a **State** via `withState()` automatically captures that **State**’s sensors, so adding them here might be redundant.

- **`#!cpp withNoDefaultEventListener()`**  
  Removes the default event handler from the event manager, useful if you have a custom one that alters default behavior.

- **`#!cpp withLogPrefixFormatting(const char *prefix)`**  
  Changes the formatting of the log prefix. You **must** use `$time` and `$logType` to reference the current log’s time and log type. (Default is `"$time - [$logType] "`.)

## Full Example

Below is a simple, hypothetical Arduino sketch illustrating how to use **MMFSSystem** with **MMFSConfig**:

```cpp
#include <Arduino.h>
#include <MMFS.h>
#include <MyCustomState.h>  // Your derived State class
#include <MyCustomReporter.h> // Additional DataReporter

// Create instances
MyCustomState myState;
MyCustomReporter myReporter;

// Create the configuration
MMFSConfig config = MMFSConfig()
    .withState(&myState)    // Required for launch detection
    .withUpdateRate(20)     // 20 Hz update
    .withOtherDataReporters({&myReporter}) // Additional data reporters
    .withLogPrefixFormatting("$time - [$logType]: ");

// Create the system object
MMFSSystem system = MMFSSystem(config);

void setup() {
    Serial.begin(115200);
    // Initialize the system
    system.init(); // (1)!
}

void loop() {
    // Update the system
    system.update();

    // Your other code goes here...
}
```
{.annotate}

1. This function will call events based on whether or not **State** and **Logger** initialize successfully. They are automatically handled by the default event listener.