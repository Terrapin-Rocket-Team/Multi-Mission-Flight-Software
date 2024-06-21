#include "ErrorHandler.h"

// Implementation of Error class
Error::Error(ErrorType type, const char *message, int errorLocation)
{
    this->type = type;
    this->message = message;
    this->errorLocation = errorLocation;
}

Error::~Error()
{
    delete[] message;
}

const char *Error::getTypeString() const
{
    switch (type) {
        case ERROR: return "ERROR";
        case WARNING: return "WARNING";
        case SOFTWARE_ERROR: return "SOFTWARE_ERROR";
        case HARDWARE_ERROR: return "HARDWARE_ERROR";
        case CRITICAL_ERROR: return "CRITICAL_ERROR";
        default: return "UNKNOWN";
    }
}

const char *Error::getMessage() const
{
    return message;
}

int Error::getErrorLocation() const
{
    return errorLocation;
}

Error *Error::getNext() const
{
    return next;
}

Error *Error::getPrev() const
{
    return prev;
}

void Error::setNext(Error *next)
{
    this->next = next;
}

void Error::setPrev(Error *prev)
{
    this->prev = prev;
}

const char *Error::toString()
{
    // Create a string with the error type, message, and location with a dynamic length
    int length = strlen(getTypeString()) + strlen(getMessage()) + snprintf(NULL, 0, "%d", getErrorLocation()) + 6;
    // Create a string with the length
    char *str = new char[length + 1];
    // Fill the string with the error type, message, and location
    snprintf(str, length + 1, "%s: %s at %d", getTypeString(), getMessage(), getErrorLocation());
    return str;
}

// Implementation of ErrorHandler class
ErrorHandler::ErrorHandler(Logger *logger)
{
    this->logger = logger;
}

ErrorHandler::~ErrorHandler()
{
    // Delete all the errors
    Error *current = errorHead;
    while (current != nullptr) {
        Error *next = current->getNext();
        delete current;
        current = next;
    }
}

void ErrorHandler::addError(ErrorType type, const char *message, int errorLocation)
{
    // Create a new error
    Error *error = new Error(type, message, errorLocation);
    
    // Add the error to the list
    addError(error);
}

void ErrorHandler::addError(ErrorType type, const char *message)
{

    // Add an error with no location
    addError(type, message, -1);
}

void ErrorHandler::addError(Error *error)
{
    // If there are no errors, make this error the head
    if (errorHead == nullptr) {
        errorHead = error;
        errorTail = error;
    } else {
        // If there are errors, add this error to the end of the list
        errorTail->setNext(error);
        error->setPrev(errorTail);
        errorTail = error;
    }
    // Log the error
    logger->logError(error);
}

Error *ErrorHandler::getFirstError()
{
    return errorHead;
}

Error *ErrorHandler::getLastError()
{
    return errorTail;
}

void ErrorHandler::printErrors()
{
    // Print all the errors
    Error *current = errorHead;
    while (current != nullptr) {
        printf("%s\n", current->toString());
        current = current->getNext();
    }
}

