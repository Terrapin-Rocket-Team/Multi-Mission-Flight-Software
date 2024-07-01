# Data Logging

## Overview

The `mmfs` library provides a flexible and efficient framework for handling data logging in embedded systems, specifically designed for flight computers on model rockets. It includes several classes that manage different storage types and logging mechanisms to ensure reliable data capture and storage throughout a flight mission.

This README covers the `StorageType`, `SdCardFile`, `PSRAM`, and `Logger` classes, providing details on their functionality, internal workings, and usage examples.

----------

## Table of Contents

-   [StorageType](#storagetype)
-   [SdCardFile](#sdcardfile)
-   [PSRAM](#psram)
-   [Logger](#logger)

----------

## StorageType

### Purpose

`StorageType` is an abstract class that defines the interface for data storage mechanisms. It provides a common framework for developers to implement their own storage solutions, ensuring consistent behavior across different storage types.

### Usage

Implementing a custom storage mechanism requires inheriting from the `StorageType` class and overriding its pure virtual functions. If you wish to implement another data storage mechanism beyond the SD card and PSRAM, it is recommended to extend this abstract class.

### Methods

-   `virtual bool init()`: Initialize the storage device.
-   `virtual bool isReady() const`: Check if the storage device is ready for read/write operations.
-   `virtual void write(char *data, int size)`: Write data to the storage device.
-   `virtual int read(char *data, int size)`: Read data from the storage device.
-   `virtual int readTo(char *data, char endChar)`: Read data until a specified end character is encountered.
-   `virtual bool seek(uint64_t offset)`: Seek to a specific offset in the storage device.

## SdCardFile

### Purpose

`SdCardFile` implements the `StorageType` interface to provide data storage on an SD card. It leverages the SdFat library to handle SD card operations, typically used on the Arduino or Teensy platform, making it easy to log flight data and events during missions. For ease of use, each method call in this class is standalone, with most methods not requiring specific calls to precede or follow itself, with the exception of the initialization logic. This also means however, that between each read or write operation, the SD card file is opened and closed. Thus, it is recommended to block together I/O calls to reduce computational overhead, and doing so in 512 byte increments, as that is what the SD card is optimized for.

If the SD card that is being used is formatted differently than the default FAT32 or FAT64 structure, change the `SD_FAT_TYPE` preprocessor variable to reflect that. This will affect the datatypes for the `sd`, `logFile`, and `flightDataFile` variables. 

### Methods

-   `bool init() override`: Initialize the SD card.
-   `bool init(const char *fileSuffix)`: Initialize the SD card with a specific file suffix.
-   `bool isReady() const override`: Check if the SD card is ready for operations.
-   `void write(char *data, int size) override`: Write data to the SD card.
-   `void print(const char *data)`: Print a null-terminated string to the SD card.
-   `void println(const char *data)`: Print a null-terminated string followed by a newline.
-   `int read(char *data, int size) override`: Read data from the SD card.
-   `int readTo(char *data, char endChar) override`: Read data until a specified end character.
-   `bool seek(uint64_t offset) override`: Seek to a specific offset.

### Example

```cpp
mmfs::SdCardFile sdCard;

if (sdCard.init("FlightData.csv")) {
    if (sdCard.isReady()) {
        char data[] = "Flight data sample";
        sdCard.write(data, strlen(data));
    }
}
```

## PSRAM

### Purpose

`PSRAM` provides a fast-access, temporary storage solution for flight data during a mission. Data is written to the PSRAM for rapid access and is later transferred to the SD card upon landing.

### Internal Mechanism

-   Uses a memory region for temporary storage.
-   Manages separate spaces for flight data and log data.
-   Data grows from the top and bottom of the memory region towards the center, allowing for 2 separate streams of data to be stored at once dynamically. 

### Methods

-   `bool init() override`: Initialize the PSRAM.
-   `void write(char *data, int size) override`: Write data to the PSRAM.
-   `void write(char *data, int size, bool writeToTop)`: Write data with control over where to write (top or bottom).
-   `void println(const char *data, bool writeToTop)`: Print a string with a newline.
-   `void print(const char *data, bool writeToTop)`: Print a string.
-   `bool isReady() const override`: Check if the PSRAM is ready for operations.
-   `int getFreeSpace() const`: Get available space in PSRAM.
-   `int getDataSpace() const`: Get space used by flight data.
-   `int getLogSpace() const`: Get space used by log data.
-   `int read(char *data, int size) override`: Read data from PSRAM.
-   `int readTo(char *data, char endChar) override`: Read data until a specified end character.
-   `int readFromBottom(char *data, int size)`: Read data from the bottom section.
-   `bool seek(uint64_t offset) override`: Seek to a specific offset.
-   `bool seekFromBottom(uint64_t offset)`: Seek from the bottom section.

### Example

```cpp
mmfs::PSRAM psram;

if (psram.init()) {
    if (psram.isReady()) {
        char data[] = "In-flight data";
        psram.println(data, true); // Write to the top
    }
}
```

## Logger

### Purpose

`Logger` combines the functionalities of `PSRAM` and `SdCardFile` to provide a comprehensive data logging solution. It manages flight data and log data, ensuring efficient storage and retrieval. If more custom data recording mechanisms are needed, this class can be avoided, however, most use cases will suffice with implementations of the `Logger` class and object.

### Internal Mechanism

-   Uses `PSRAM` for fast in-flight data logging.
-   Transfers data from `PSRAM` to `SD Card` upon landing.
-   Supports logging to USB, SD Card, or both.

### Enums
The enums within the `Logger` class allow the customization of behavior, in terms of how data is logged, as well as where it is logged to, by simply passing in the appropriate enum to the methods of this class. This way, the behavior and staging logic can be processed externally, yet the data logging logic can take in its variety of inputs. This is also used by the `ErrorHandler` class in order to log any errors or warnings that may be occurring. 

#### `LogType`
Used to prefix and indicate the type of data that the entry into the log file is. This can be of a `LOG`, `ERROR`, `WARNING`, or `INFO`, and is simply a label associated with the given entry.

#### `Dest`
Used to determine where the log data or flight data would be stored to. `TO_USB` means that the data isn't actually stored and is just printed to the serial (useful for debugging), while the `TO_FILE` option makes it so that the passed in data is only sent to the PSRAM or SD card, but doesn't clutter up the Serial output. The default behavior, `BOTH`, makes it so that the data is sent to as many destinations as possible, both printing it and recording it if viable.

#### `Mode`
Determines the mode, or more generally, the data storage approach being taken by the logger. These enums are simply monikers for these behaviors, but may be used without regard for the context of what their name may imply, if differential behavior is desired.

`GROUND` is used for slower I/O access that will persist, as all data storage and log data will be written directly to the SD card. This opens and closes the SD card files everytime it is called, and thus if used in a high clock speed environment, may become blocking. `FLIGHT` is used for faster I/O operations, as data is temporarily stored to the PSRAM. Flight data is written to the top of the PSRAM and grows downwards, while the log data is written to the bottom of the PSRAM's memory block and grows upwards. Thus, `FLIGHT` mode datalogging is restricted to a maximum of the PSRAM's available size. 

Switching from `FLIGHT` to `GROUND` mode is what dumps all data from the PSRAM to the SD card. Thus, in order to have persistent data storage, it is important to **always switch the `Logger` mode upon landing**.

### Methods

-   `Logger()`: Constructor that initializes the logger.
-   `void recordFlightData(char *data)`: Record flight data.
-   `void recordLogData(LogType type, const char *data, Dest dest = BOTH)`: Record log data.
-   `void recordLogData(double timeStamp, LogType type, const char *data, Dest dest = BOTH)`: Record log data with a timestamp.
-   `void setRecordMode(Mode mode)`: Set the recording mode (FLIGHT or GROUND).

### Example

```cpp
mmfs::Logger logger;

logger.setRecordMode(mmfs::FLIGHT);
char flightData[] = "Altitude: 1000m";
logger.recordFlightData(flightData);

logger.setRecordMode(mmfs::GROUND);
char logData[] = "Mission completed.";
logger.recordLogData(mmfs::INFO, logData);
```
