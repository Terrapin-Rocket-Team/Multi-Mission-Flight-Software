#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <stdio.h>
#include <cstring>
#include "../RecordData/Logger.h"

class BBPattern; // foward declaration

namespace mmfs
{

    /** Enum for the different types of errors */
    enum ErrorType
    {
        GENERIC_ERROR,
        NONCRITICAL_WARNING,
        SOFTWARE_ERROR,
        HARDWARE_ERROR,
        CRITICAL_ERROR
    };

    /**
     * Class representing an error, as a node in a linked list.
     */
    class Error
    {
    private:
        Error *next = nullptr;
        Error *prev = nullptr;

    public:
        ErrorType type;      /** Type of the error. */
        const char *message; /** Error message. */
        int errorLocation;   /** Location where the error occurred, or just a number designator. */

        /**
         * Constructor for the Error class.
         * @param type The type of the error.
         * @param message The error message.
         * @param errorLocation The location where the error occurred, or just a number designator for debugging.
         */
        Error(ErrorType type, const char *message, int errorLocation);
        ~Error();

        /**
         * Get the string representation of the error type.
         * @return The string representation of the error type.
         */
        const char *getTypeString() const;

        /**
         * Get the error message.
         * @return The error message.
         */
        const char *getMessage() const;

        /**
         * Get the location where the error occurred.
         * @return The error location.
         */
        int getErrorLocation() const;

        /**
         * Get the next error in the linked list.
         * @return Pointer to the next error.
         */
        Error *getNext() const;

        /**
         * Get the previous error in the linked list.
         * @return Pointer to the previous error.
         */
        Error *getPrev() const;

        /**
         * Set the next error in the linked list.
         * @param next Pointer to the next error.
         */
        void setNext(Error *next);

        /**
         * Set the previous error in the linked list.
         * @param prev Pointer to the previous error.
         */
        void setPrev(Error *prev);

        /**
         * Get a string representation of the error.
         * @return A string representation of the error.
         */
        const char *toString();
    };

    /**
     * Singleton class for handling errors. Stores errors in a linked list.
     */
    class ErrorHandler
    {

    public:
        // Make a linked list of errors
        Error *errorHead = nullptr;
        Error *errorTail = nullptr;

        /**
         * Constructor for the ErrorHandler class.
         */
        ErrorHandler();
        ~ErrorHandler();

        /**
         * Add an error to the error handler.
         * @param type The type of the error.
         * @param message The error message.
         * @param errorLocation The location where the error occurred, or just a number designator for debugging.
         */
        void addError(ErrorType type, const char *message, int errorLocation);

        /**
         * Add an error to the error handler without specifying the error location.
         * @param type The type of the error.
         * @param message The error message.
         */
        void addError(ErrorType type, const char *message);

        /**
         * Add an existing error object to the error handler.
         * @param error Pointer to the error object.
         */
        void addError(Error *error);

        /**
         * Add an error to the error handler with a pin number and BlinkBuzz pattern.
         * @param type The type of the error.
         * @param message The error message.
         * @param errorLocation The location where the error occurred, or just a number designator for debugging.
         * @param pinNum The pin number to output to using BlinkBuzz.
         * @param pattern The BlinkBuzz pattern associated with the error.
         */
        void addError(ErrorType type, const char *message, int errorLocation, int pinNum, BBPattern *pattern);

        /**
         * Get the first error in the linked list.
         * @return Pointer to the first error.
         */
        Error *getFirstError();

        /**
         * Get the last error in the linked list.
         * @return Pointer to the last error.
         */
        Error *getLastError();

        /**
         * Print all the errors in the linked list.
         */
        void printErrors();
    };

} // namespace mmfs

extern mmfs::ErrorHandler errorHandler;

#endif // ERROR_HANDLER_H
