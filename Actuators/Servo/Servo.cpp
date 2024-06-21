#include "Servo.h"

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
{ // See State.cpp::setDataString() for comments on what these numbers mean
    // float x1
    const int size = 12 * 1 + 3;
    char *data = new char[size];
    snprintf(data, size, "%.2f,", angle); // trailing comma
    return data;
}