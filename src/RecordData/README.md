# Data Logging

This is a comprehensive data logging system for recording flight and log data during flights. It provides access to the PSRAM and SD card, and provides a logging utility to handle storing data before, during, and after flight.

Contents:

  - [Usage](#usage)
    - [Initialization](#initialization)
    - [Setup](#setup)
    - [Logging](#logging)
    - [Flight Mode](#flight-mode)
  - [Internal Systems](#internal-systems)
    - [`SdFat.h`](#sdfath)
    - [`PSRAM.h`](#psramh)
    - [`PSRAMFile.h`](#psramfileh)
    - [`DataReporter.h`](#datareporterh)

# Usage

Let's start with how to use the system in your own code, using the logger utility.

## Initialization

To start, there are a few singletons declared in the various RecordData files. You'll need to provide definitions for these in your main file. The logger object takes two arguments:
- `bufferTime` - How long the pre-flight buffer should be in seconds. Set this to `0` to disable the pre-flight buffer.
- `bufferInterval` - How often to write buffer data to the SD card. (in seconds) This way you can store, for example, 30 seconds of 20hz data in the buffer, but only write to the SD card every 30 seconds on the pad. This is useful at removing mostly useless pre-flight data from the flight file, but still allows you to see macro sensor trends before the flight. Set this to `0` to write every line to the SD card, or `-1` to disable writing pre-flight data to the SD card entirely.

There is also a `PSRAM *` that needs to be initialized. I recommend doing this in the `setup()` function of your main file, as there are other variables that can be changed before setting up the PSRAM object.

```cpp
#include "RecordData/Logger.h"

// Define the logger singleton
Logger logger; //using defaults

//OR

Logger logger(30, 30); //30 second buffer, write one line every 30 seconds to the SD card
```

## Setup

In the `setup()` function of your main file, you'll need to initialize the PSRAM object and call `logger.init(State *stateInstance)`. This will set up the PSRAM object and the SD card, and create the files on the SD card. You need to pass in a pointer to the state object so that the logger can access the state object to get the format of the data it needs to log.

This is also where you would change the values of the "File Constants" in `Constants.h`. You would put the new values in setup(). I recommend not changing the values if you don't need to.

```cpp
void setup() {
    //Example changing the number of max PSRAM files
    MAX_PSRAM_FILES = 10; // up from 5. logger currently only uses 3, but if you need or want more for some reason, you can increase it.

    //After changing whatever values you're going to, create an instance of the PSRAM object
    psram = new PSRAM();

    //make a state object to use for the rest of the program
    state = new State(...); //whatever you need to pass in for your version of state

    //initialize the logger
    logger.init(state);

    // Now, you can check the state of the SD card and PSRAM

    if(logger.isSdCardReady())
        Serial.println("SD Card Ready");
    else
        Serial.println("SD Card Not Ready");
    
    if(logger.isPsramReady())
        Serial.println("PSRAM Ready");
    else
        Serial.println("PSRAM Not Ready");

    //next, initialize state
    state->init();

    // finally, you can send the csvHeader to the flight file
    logger.writeCsvHeader();
}
```

## Logging

Now that the object is set up, you can log data using it. There are two files the logger uses to store data. A flight data file, intended to store csv data from the state and sensors, and a log file, intended to log events or errors.

You log the flight data in the `loop()` function of your main file.

```cpp

void loop() {
    //update the state
    state->updateState();

    //log the state
    logger.recordFlightData();
}
```

You can log events or errors to the log file using the `recordLogData()` function. This will write a line to the log file with the current time and the message you provide.

There are up to 4 arguments:
- `timeStamp` - (optional) The time to log the message at. If you don't provide a time, it will use the current time.
- `logType` - The type of log message. This is an enum with the following values:
    - `LOG_` - General information
    - `WARNING_` - A warning
    - `ERROR_` - An error
    - `INFO_` - Information
- `message` - The message to log as a char array
- `destination` - (optional) Another enum, telling it where to send the data:
    - `TO_FILE` - Write to the SD card
    - `TO_USB` - Write to the serial monitor
    - `BOTH` - Write to both the SD card and the serial monitor (default)

```cpp
logger.recordLogData(WARNING_, "Warning: Something happened");

// OR

logger.recordLogData(ERROR_, "Error: Something went wrong", TO_USB); //only write to the serial monitor

// OR

logger.recordLogData(5, INFO_, "Information: Something happened", TO_FILE); //only write to the SD card with timestamp of "5.000"
```

As long as you include the `RecordData/Logger.h` file, you can use these logging statements anywhere in your code to note events or errors.

## Flight Mode

The logger has a flight mode that you can set it to stop using the buffer file and start recording every piece of data. When you detect launch, you can call `logger.setRecordMode(FLIGHT)` to start recording all data to the psram's flight file. When the flight is finished, call `logger.setRecordMode(GROUND)` to write all gathered data to the SD card.

IMPORTANT NOTE: The logger will not write any data to the SD card until you call `logger.setRecordMode(GROUND)`. If you don't call this, you will lose all data gathered during the flight. This is to prevent the logger from writing data to the SD card during the flight, which can slow down the flight controller.

---

# Internal Systems

Now, I'll go into the internal systems of the logger, and how they work. It's a bit messy as there are many flags that need to be accounted for that change the way the logger behaves.

## `SdFat.h`

This is a built-in arduino library that interfaces with the SD card. The logger holds two `SdFs` objects that represent the two data files (log and flight data) on the SD card.

## `PSRAM.h`

This is a custom library that interfaces with the PSRAM. The logger uses the global `psram` pointer to access the PSRAM object.

PSRAM has a custom, basic file system to store data in "files". It works a bit like the FAT system, but worse because it's custom. It interfaces with the [PSRAMFile](#psramfileh).

Internally, it gets its size from an external variable that arduino feeds it. It can store up to 256 clusters, with a size set by the `PSRAM_CLUSTER_SIZE` in `Constants.h`. It has a cluster map, which is an array of 32 bit ints that it uses to allocate space for files. Each 32-bit integer is broken up into eight 4-bit values. Here, each file forms a doubly-linked list indicating which cluster is next in the file and which one came before it.
- The first 4 bits are the previous cluster
- The second 4 bits are the next cluster
- The next 8 bits represent the ID of the file, which is used to find the file in the file map
- The last 8 bits are unused.

Note: The 256 cluster limit is pretty low, but increasing it means using as much as twice the memory for the cluster map, which is already 256 * 4 bytes.

There is also a file map, which is an array of pointers. The index of the array is the file id, and the value is the pointer in PSRAM memory to that file.

There are a few private methods to invoke read and write operations, that are called by the PSRAMFile object.

For example, when you call `file.println("some characters")`, it essentially calls `psram->writeFile(this->fileId, "some characters")` to write the data to the file.

There is one interesting method for reading files, called `readNextFileCluster()`. Because the PSRAM is just raw memory addresses, we can directly read from it without needing to copy the data to RAM. This method simply returns the starting address of the next cluster in the file, and provides the size of the file data in that cluster (`max(PSRAM_CLUSTER_SIZE, file size)`). This avoids needing to allocate massive amounts of ram to read a file in order, which is useful for copying large files (like the flight data file) to the SD card.

## `PSRAMFile.h`

This is a relatively standard file implementation. You can read, write, and seek through files. This is where the simplicity of this file system really shows, as you can't really close or delete a file.

Most of the methods here simply call the related methods in the PSRAM object. For example, `println()` calls `psram->writeFile()` with some additional formatting.

The logger object uses this file system to store temporary data on the PSRAM. To prevent slow SD card writes from blocking in-flight mcu logic, it stores all data gathered during flight in the PSRAM, and writes it to the SD card after the flight is over.

## `DataReporter.h`

This is a basic interface that all sensors and the state implement. It provides access to a `packData()` method that is used to compress the flight data when stored on the PSRAM. This is useful for storing large amounts of data in a small space, as the PSRAM is limited in size.

The packed data is stored essentially as a struct, but literally as a byte array called `packedData`. There are helper methods that allow other classes to know the proper format and type of the data:

- `getPackedDataSize()` - returns the size of the packed data array
- `getPackedOrder()` - uses an enum to return the order of the data in the packed data array (ex: {DOUBLE, INT, INT, FLOAT, ...}). This allows unpacking the data later by understanding the size and format of the raw stored bytes.
- `getNumPackedDataPoints()` - essentially the length of the packed order array. The number of data points being stored in the packed data array.
- `getPackedDataLabels()` - returns an array of strings that represent the data points in the packed data array. This is useful for writing the csv header to the flight file.
- `getPackedData()` - returns the packed data array.

Those are the publicly facing methods, but there is also the internal `packData()` method that needs to be overridden by the classes that implement the interface. This method is responsible for taking the raw data from the sensors and state and compressing it into the packed data array. This is where the order of the data is set, and the data is stored in the array.

The `getPackedOrder()`, `getPackedDataLabels()`, and `getNumPackedDataPoints()` methods also need to be overridden by the implementing classes. These methods are used by the logger to understand the format of the data being stored, and to write the csv header to the flight file.

As I mentioned, State also implements this interface, so that it can spit out its data from a filter or other logic and record it to the flight data file.

## `DataFormatter.h`

This is a static helper class that the logger uses to pack and unpack data from the sensors. It can convert data to and from a byte array, and can also convert the byte array to a csv string for writing to the flight file.

## `Logger.h`

Finally, the meat of the RecordData system. This is the class that actually *does* all of the file management and data logging.

To start, here is an overview of the methods:

- `init(State *stateInstance)` - Initializes the PSRAM and SD card, and creates the necessary files in both. It needs the state object to get the format of the data it needs to log. It iterates through the files on the SD card and increments the flight # to the next available number. It also logs which version of MMFS is being used. (taken from the library.json file)

- `recordFlightData()` - Iterates through the sensors belonging to `state` and records their most recent data.

- `recordLogData()` - Records a log message to the log file. This is useful for recording events or errors that occur during flight.

- `setRecordMode()` - Changes whether it uses the buffer file or the flight file. This is useful for starting and stopping the recording of data during flight. DOES NOT SUPPORT switching to flight mode more than once.

- `dumpData()` - Dumps the data from the PSRAM to the SD card. Called when you call `setRecordMode(GROUND)`.

- `writeCsvHeader()` - Writes the csv header to the flight file. This should be called in the `setup()` function of your main file.

## Deeper look at internal logic

The logger has a few flags that change the way it behaves:

- `recordMode`  - either GROUND or FLIGHT, changes the way the logger records data.
- `packData` - This is set to true if you elect to use a buffer for pre-flight data. It determines whether or not the logger simply records csv data or if it compresses it.
- `bufferInterval` - This changes how the logger handles data before launch. If it's set to 0, it will write every line to the SD card. If it's set to -1, it will not write any pre-flight data to the SD card. If it's set to any other value, it will write a line to the SD card every `bufferInterval` seconds.

See the `Logger.h` file for a more detailed breakdown of the logic with comments.

# Final Notes

When implementing a new type of sensor, or overriding the provided types, don't forget to provide the pack data methods, and to call `packData()` in the update step.

Also, remember to call `recordFlightData()` in the `loop()` function of your main file to record the data to the flight file.

Finally, remember to call `setRecordMode(GROUND)` after the flight is over to write all the data to the SD card. If you don't, you will lose all the data gathered during the flight.