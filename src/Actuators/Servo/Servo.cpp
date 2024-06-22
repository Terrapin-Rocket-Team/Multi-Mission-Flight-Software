#include "Servo.h"

using namespace mmfs;

bool Servo::initialize()
{
    initialized = true;
    return true;
}

void Servo::setAngle(double servoAngle)
{
    // Angle must be in degrees from [0:180]
    if (servoAngle >= 0 && servoAngle <= 180)
    {
        angle = servoAngle;
        servoObject.write(angle);
    }
    else
    {
        // TODO error
    }
}

void Servo::setSignalPin(int pin)
{
    signalPin = pin;
    servoObject.attach(signalPin);
}

double Servo::getAngle() const
{
    return angle;
}

const char *Servo::getCsvHeader() const
{
    return "Angle (deg),"; // trailing commas are very important
}

const char *Servo::getDataString() const
{
    snprintf(data, (MAX_DIGITS_FLOAT+BUFFER_SIZE), "%.2f,", angle); // trailing comma
    return data;
}