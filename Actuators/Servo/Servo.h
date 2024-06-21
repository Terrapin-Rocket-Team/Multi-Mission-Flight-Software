#ifndef SERVO_H
#define SERVO_H

#include "../Actuator.h"
#include <PWMServo.h>

class Servo : public Actuator
{
public:
    // Constructor to initialize staticData and data
    Servo(const char servoName[], int pin)
    {
        data = new char[DATA_SIZE + BUFFER_SIZE];

        setName(servoName);
        setSignalPin(pin);
        initializeServo();
    }

    // Destructor to free allocated memory
    virtual ~Servo()
    {
        delete[] data;
    }

    void initializeServo();
    void setSignalPin(int pin);
    const char *getName() const;
    virtual void setAngle(double servoAngle); // Angle in degrees
    virtual double getAngle() const;
    virtual const char *getCsvHeader() const override;
    virtual const char *getDataString() const override;
    virtual const char *getTypeString() const override { return "Servo"; }
    virtual SensorType getType() const override { return SERVO_; }

protected:
    Servo(){}; // Protected constructor to prevent instantiation
    int signalPin;
    double angle = 0;
    PWMServo servoObject;

private:
    char *data;
    static const int DATA_SIZE = 12; // Assuming 12 bytes for the floats
    static const int BUFFER_SIZE = 10;
};

#endif // SERVO_H