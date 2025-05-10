# Logger

!!! note
    It is very likely that this class has minor changes to to the way it is set up in the future. Please verify that the website footer indicates the correct MMFS version number.

The **Logger** class is one of the most powerful, but also one of the most complicated systems in **MMFS**. It can take both log data and flight data and write them to onboard storage or USB. ~~It (by default) performs less frequent writes during pre- and post-flight periods to prevent writing unnecessary data.~~ Additionally, it can modify the date of file creation so it is accurate once an onboard GPS obtains a time fix (Not currently working). Finally, if it crashes, it will log the reason for the crash the next time it boots.

With all this functionality, there are numerous configuration options to choose from, so feel free to jump to a specific section using the table of contents on the right.

## Setup

The first thing to know about the **Logger** class is that it exists as a singleton, accessed via the global `getLogger()` method. Every time you need to use any of the **Logger**’s functions, you should prefix them with `getLogger()` followed by the function call.

We **strongly** recommend using the `MMFSSystem` and `MMFSConfig` objects whenever you use MMFS, as they handle almost all initialization tasks for you. However, all utilities can be used outside of `MMFSSystem`, including **Logger**. The only difference between these two approaches is the setup. All other functions are identical.

/// tab | With MMFSSystem

Here is a list of the `MMFSConfig`[^1] options relevant to **Logger**:

These defaults should be fine for most use cases. The defaults are shown below:

```cpp
#include <MMFS.h>

MMFSConfig config = MMFSConfig()
                    .withOtherDataReporters(DataReporter **others) // Add additional objects that can report flight data
                    .withLogPrefixFormatting("$time - [$logType] "); // Change format string for log data (must include $time and $logType)
```

///

/// tab | Without MMFSSystem

If you simply want to use **Logger** without `MMFSSystem`, you need to call `getLogger().init()` with the default parameters. You must pass all the DataReporter objects (including State and all its sensors) and their count. You can also pass in the buffer time and buffer interval variables. Check if it worked with `getLogger().isReady()`. At the end of setup (after all of the data reporters are initialized), you should call `getLogger().writeCsvHeader()` to write the initial line to the SD card.

```cpp
#include <Logger.h>

setup() {
    // Example:
    DataReporter *reporters[] = { &state, &sensor1, &sensor2, ... };
    getLogger().init(reporters, numberOfReporters, 30, 30);
    if (getLogger().isReady()) {
        getLogger().writeCsvHeader();
    }
}
```

///

---

## Recording Log Data

### Before You Begin

All log data is recorded to a file called `###_Log.txt`, where ### is an incrementor based on files already existing in the file system (So largest # = newest file). It prefixes all log entries with a timestamp and a log type. The log types are:

- **`INFO_`**: General system information (default).
- **`WARNING_`**: A warning about a potential issue.
- **`ERROR_`**: An error that has occurred.
- **`LOG_`**: A log entry that is not an error or warning.
- **`CUSTOM_`**: A log entry with a custom prefix.

You can change the format of the default log types before calling `getLogger().init()` by calling:

```cpp
getLogger().setLogPrefixFormatting(const char *prefix);
```

The default is `"$time - [$logType] "`, which will produce something like `0.000 - [INFO] Hello, world!`. When you call this function, you **must** include the keywords `$time` and `[$logType]`. `$time` is replaced with the time (in seconds) since the microcontroller turned on (printed with 3 decimal places), and `[$logType]` is replaced with the log type stamp. You can include any additional text you like.

You may change the log prefix for the custom log type by calling:

```cpp
getLogger().setCustomLogPrefix(const char *prefix);
```

The default is `"$time - [CUSTOM] "`, which would output something like `0.000 - [CUSTOM] Hello, world!`. Including `$time` is recommended but not required. This method can be called at any time, and the custom prefix will remain until overridden by a subsequent call.

When you record log data, you can send it to the SD card, USB serial, or both. The default behavior is to record to both. These options exist in the `Dest` enum:

```cpp
enum Dest {
    TO_FILE,
    TO_USB,
    BOTH
};
```

### Recording Data

Once setup is complete, you can record log data using:

```cpp
getLogger().recordLogData(...);
```

There are eight different function overloads for this method, each taking different arguments. Essentially, you specify the log type, where you want to send it (destination), and the actual data you want to log. You can optionally supply a timestamp instead of letting it be generated automatically, and you can choose to use or not use a printf-style format string followed by any number of arguments. Rather than list every overload, here are a couple of examples:

```cpp
getLogger().recordLogData(WARNING_, TO_USB, "Hello, world!");
getLogger().recordLogData(INFO_, TO_FILE, 50, "Hello %s", "there!");
```

!!! note
    If you use the format string versions of the method, you **must** provide an integer representing the maximum length you expect the string to be once formatted (50 in this example).

Once the data is logged to its destination, **Logger** will fire a `LogData` event with the ID `"LOG_DATA"_i`[^2]. You can listen for this event if you want to do something additional with the logged data.

---

## Recording Flight Data

### Before You Begin

Flight data is stored in two files: a preflight data file and a flight data file. ~~By default, it only stores pre- and post-flight data once every 30 seconds. This is why having a robust **State** with proper launch detection is important. By default, it also stores the most recent 30 seconds of data before launch at the full data rate. You can change this behavior by modifying the buffer time and buffer interval parameters in the init function. To write all data directly to the SD card, set the buffer interval to zero. To write no data to the SD card and store everything in PSRAM until landing is detected, set the buffer interval to less than zero.~~

The Logger currently supports two types of file storage hardware. a MicroSD card, plugged into the slot on the Teensy, and QSPI NAND Flash, soldered onto the bottom of the board. The logger prefers using the flash memory for robustness reasons, but will fallback to the SD card if flash is not detected.

All functionality surrounding flight and preflight data is managed by an internal enum called `Mode`. By default, the mode is `GROUND`. Once you call:

```cpp
getLogger().setMode(FLIGHT);
```

the Logger assumes the rocket has just launched and switches into flight mode. When landing is detected, simply call:

```cpp
getLogger().setMode(GROUND);
```

and the Logger will switch back to pre-/post-flight data mode.

### Recording Data

If you use the **MMFSSystem** objects, this is done automatically when you call `system.update()` in your loop. If you are not using the **MMFSSystem** objects, you must call:

```cpp
getLogger().recordFlightData();
```

in your loop to record flight data. This method automatically detects the mode and logs the appropriate data. The order of columns in the CSV file is determined by the order of the data reporter objects passed to the Logger. If you use **MMFSSystem**, this will be the **State** followed by all sensors in the order they were passed to the **State**, then anything else added later.

---

## Miscellaneous

### Changing the Creation Date of Files

Unfortunately, the SD card itself has no concept of real-world time. However, most flight systems have a GPS onboard, and by taking advantage of the default GPS event, **MMFS** will attempt to set correct creation and modification dates for all files on the SD card, making them easier to manage later. This is handled by default through the default event handler[^2]. If you disabled the default event handler, you can still modify file dates by calling:

```cpp
getLogger().modifyFileDates(GPS *gps);
```

### CrashReport

This is another feature of **Logger**. It has no configuration options, but it is helpful to have. If the Teensy detects a crash, a crash report object is created, and the Logger will attempt to record it in the `###_Log.txt` file on the next boot.

[^1]: [MMFSConfig](mmfssys.md#mmfsconfig) from [MMFSSystem](mmfssys.md)  
[^2]: [Event](event.md)

