#ifndef ERROR_H
#define ERROR_H

// enum for the different types of errors
enum ErrorType
{
    ERROR,
    WARNING,
    SOFTWARE_ERROR,
    HARDWARE_ERROR
};

class Error
{
private:
    int errorLocation;

public:

    ErrorType type;
    const char *message;

    Error(ErrorType type, const char *message, int errorLocation)
    {
        this->type = type;
        this->message = message;
        this->errorLocation = errorLocation;
    }

    Error(ErrorType type, const char *message)
    {
        this->type = type;
        this->message = message;
    }

    Error(const char *message)
    {
        this->type = ERROR;
        this->message = message;
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
        else
        {
            return "UNKNOWN";
        }
    }

    const char *getMessage() const
    {
        return message;
    }
};

#endif ERROR_H