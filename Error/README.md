# Error Handling

## Overview

This library provides a robust error handling mechanism designed for embedded systems, specifically tailored for flight computers on model rockets. The core component of this library is the `ErrorHandler` class, which enables efficient tracking, logging, and management of errors through a linked list of `Error` objects. This ensures that errors are systematically recorded, making it easier to diagnose and address issues that arise during operation.

## Why Use `ErrorHandler`

In the context of embedded systems, particularly those used in flight computers for model rockets, real-time error tracking and management are crucial. The `ErrorHandler` class:

-   Provides a structured way to log errors with varying severity levels.
-   Supports easy retrieval and printing of errors for debugging purposes.
-   Integrates with the `Logger` class to persist errors for post-mission analysis.
-   Helps maintain system stability by categorizing errors and enabling appropriate handling mechanisms.

## Internals

### ErrorType Enum

The `ErrorType` enum defines the different categories of errors that can occur:
```cpp
enum ErrorType {
    GENERIC_ERROR,
    NONCRITICAL_WARNING,
    SOFTWARE_ERROR,
    HARDWARE_ERROR,
    CRITICAL_ERROR
};
```
### Error Class

The `Error` class represents an individual error as a node in a linked list. Each error includes details such as type, message, and location.

Key Methods:

-   `getTypeString()`: Returns a string representation of the error type.
-   `getMessage()`: Retrieves the error message.
-   `getErrorLocation()`: Returns the location where the error occurred.
-   `toString()`: Provides a formatted string representation of the error.

### ErrorHandler Class

The `ErrorHandler` class manages a linked list of `Error` objects, providing methods to add, retrieve, and print errors. This way, a variable amount of errors or warnings can be stored.

Key Methods:

-   `addError(type, message, errorLocation)`: Adds a new error with a specified location.
-   `addError(type, message)`: Adds a new error without specifying a location.
-   `addError(error)`: Adds an existing `Error` object to the list.
-   `getFirstError()`: Retrieves the first error in the list.
-   `getLastError()`: Retrieves the last error in the list.
-   `printErrors()`: Prints all errors in the list.

## Usage

### Initialization

To use the `ErrorHandler`, you need to initialize it with a `Logger` instance. This allows errors to be logged to the log file, held in either the PSRAM during flight, or on the SD card elsewise.

```cpp
#include "ErrorHandler.h"
#include "Logger.h"

mmfs::Logger logger;
mmfs::ErrorHandler errorHandler(&logger);
```

Since the `ErrorHandler` class is a singleton, only once instance of it exists and needs to be created. This instance is declared as an extern in the header file, so it can be accessed from any file that includes the header, however, it must be initialized in the main file.

### Adding Errors

You can add errors with or without specifying an error location. The int parameter can also be used as a number demarcation to help with debugging by serving as an identifier.
```cpp
// Adding an error with a location
errorHandler.addError(mmfs::SOFTWARE_ERROR, "Null pointer dereferenced", 42);

// Adding an error without specifying a location
errorHandler.addError(mmfs::HARDWARE_ERROR, "Sensor failure");` 
```

### Retrieving Errors

Retrieve the first or last error in the handler:
```cpp
mmfs::Error *firstError = errorHandler.getFirstError();
mmfs::Error *lastError = errorHandler.getLastError();` 
```
### Printing Errors

Print all recorded errors to the console:
```cpp
errorHandler.printErrors();
``` 

### Example

Here's a complete example demonstrating how to use the `ErrorHandler`:

```cpp
#include "ErrorHandler.h"
#include "Logger.h"

int main() {
    // Initialize logger and error handler
    mmfs::Logger logger;
    mmfs::ErrorHandler errorHandler(&logger);

    // Add errors
    errorHandler.addError(mmfs::GENERIC_ERROR, "General error occurred", 101);
    errorHandler.addError(mmfs::NONCRITICAL_WARNING, "Low battery warning");
    errorHandler.addError(mmfs::SOFTWARE_ERROR, "Memory allocation failure", 202);

    // Retrieve and print errors
    mmfs::Error *firstError = errorHandler.getFirstError();
    if (firstError) {
        printf("First Error: %s\n", firstError->toString());
    }

    mmfs::Error *lastError = errorHandler.getLastError();
    if (lastError) {
        printf("Last Error: %s\n", lastError->toString());
    }

    // Print all errors
    errorHandler.printErrors();
}
```