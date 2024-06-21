#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <stdio.h>
#include <cstring>

// enum for the different types of errors
enum ErrorType
{
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

        Error(ErrorType type, const char *message, int errorLocation)
        {
            this->type = type;
            this->message = message;
            this->errorLocation = errorLocation;
        }

        const char *getTypeString() const
        {
            if (type == ERROR)
            {
                return "ERROR";
            }
            else if (type == WARNING)
            {
                return "WARNING";
            }
            else if (type == SOFTWARE_ERROR)
            {
                return "SOFTWARE_ERROR";
            }
            else if (type == HARDWARE_ERROR)
            {
                return "HARDWARE_ERROR";
            }
            else if (type == CRITICAL_ERROR)
            {
                return "CRITICAL_ERROR";
            }
            else
            {
                return "UNKNOWN";
            }
        }

        const char *getMessage() const
        {
            return message;
        }

        int getErrorLocation() const
        {
            return errorLocation;
        }

        Error *getNext() const
        {
            return next;
        }

        Error *getPrev() const
        {
            return prev;
        }

        void setNext(Error *next)
        {
            this->next = next;
        }

        void setPrev(Error *prev)
        {
            this->prev = prev;
        }

        const char* toString() {
            // create a string with the error type, message, and location with a dynamic length
            // find length of the string
            int length = strlen(getTypeString()) + strlen(getMessage()) + snprintf(NULL, 0, "%d", getErrorLocation()) + 6;
            // create a string with the length
            char *str = new char[length + 1];
            // fill the string with the error type, message, and location
            snprintf(str, length + 1, "%s: %s at %d", getTypeString(), getMessage(), getErrorLocation());
            return str;
        }

        ~Error() {
            delete[] message;
        }

};

class ErrorHandler
{
private:
    Logger *logger;

public:
    // make a linked list of errors
    Error *errorHead = nullptr;
    Error *errorTail = nullptr;

    ErrorHandler(Logger *logger)
    {
        this->logger = logger;
    }

    void addError(ErrorType type, const char *message, int errorLocation)
    {
        // create a new error
        Error *error = new Error(type, message, errorLocation);
        // if there are no errors, make this error the head
        if (errorHead == nullptr)
        {
            errorHead = error;
            errorTail = error;
        }
        // if there are errors, add this error to the end of the list
        else
        {
            errorTail->setNext(error);
            error->setPrev(errorTail);
            errorTail = error;
        }
        // log the error
        logger->logError(error);
    }

    Error *getFirstError()
    {
        return errorHead;
    }

    Error *getLastError()
    {
        return errorTail;
    }

    void printErrors()
    {
        // print all the errors
        Error *current = errorHead;
        while (current != nullptr)
        {
            printf("%s\n", current->toString());
            current = current->getNext();
        }
    }

    ~ErrorHandler()
    {
        // delete all the errors
        Error *current = errorHead;
        while (current != nullptr)
        {
            Error *next = current->getNext();
            delete current;
            current = next;
        }
    }
    
};

extern ErrorHandler *errorHandler;

#endif // ERROR_HANDLER_H