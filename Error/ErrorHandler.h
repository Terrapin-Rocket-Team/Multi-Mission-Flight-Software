#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <stdio.h>
#include <cstring>

// Enum for the different types of errors
enum ErrorType {
    ERROR,
    WARNING,
    SOFTWARE_ERROR,
    HARDWARE_ERROR,
    CRITICAL_ERROR
};

class Error {
private:
    Error *next = nullptr;
    Error *prev = nullptr;

public:
    ErrorType type;
    const char *message;
    int errorLocation;

    Error(ErrorType type, const char *message, int errorLocation);
    ~Error();

    const char *getTypeString() const;
    const char *getMessage() const;
    int getErrorLocation() const;
    Error *getNext() const;
    Error *getPrev() const;
    void setNext(Error *next);
    void setPrev(Error *prev);
    const char *toString();
};

class ErrorHandler {
private:
    Logger *logger;

public:
    // Make a linked list of errors
    Error *errorHead = nullptr;
    Error *errorTail = nullptr;

    ErrorHandler(Logger *logger);
    ~ErrorHandler();

    void addError(ErrorType type, const char *message, int errorLocation);
    void addError(ErrorType type, const char *message);
    void addError(Error *error);
    Error *getFirstError();
    Error *getLastError();
    void printErrors();
};

extern ErrorHandler errorHandler;

#endif // ERROR_HANDLER_H
