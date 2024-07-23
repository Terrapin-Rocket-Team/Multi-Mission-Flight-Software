
#include "BlinkBuzz.h"

BlinkBuzz::BlinkBuzz(int *allowedPins, const int numPins, bool enableAsync)
{
    this->enableAsync = enableAsync;
    this->numPins = numPins;
    this->allowedPins = allowedPins;
    pinState = new bool[numPins];
    if (enableAsync)
    {
        pinQStart = new int[numPins];
        pinQEnd = new int[numPins];
        pinQIndef = new int[numPins];
        pinQ = new int *[numPins];
        for (int i = 0; i < numPins; i++)
        {
            pinQStart[i] = 0;
            pinQEnd[i] = 0;
            pinQIndef[i] = -1; // -1 means no indefinite pattern
            pinQ[i] = new int[MAX_QUEUE];
        }
    }
    else
    {
        pinQ = nullptr;
        pinQStart = nullptr;
        pinQEnd = nullptr;
        pinQIndef = nullptr;
    }
    for (int i = 0; i < numPins; i++)
    {
        pinState[i] = false;
#ifdef ARDUINO
        pinMode(allowedPins[i], OUTPUT);
#endif // ARDUINO
    }
}
BlinkBuzz::~BlinkBuzz()
{
    delete[] pinState;
    if (enableAsync)
    {
        delete[] pinQStart;
        delete[] pinQEnd;
        delete[] pinQIndef;
        for (int i = 0; i < numPins; i++)
        {
            delete[] pinQ[i];
        }
        delete[] pinQ;
    }
}

#pragma region Synchronous and Helper functions

bool BlinkBuzz::isAllowed(int pin)
{
    if (pin < 0)
        return false;

    for (int i = 0; i < numPins; i++)
        if (allowedPins[i] == pin)
            return true;
    return false;
}
int BlinkBuzz::getPinIndex(int pin)
{
    for (int i = 0; i < numPins; i++)
        if (allowedPins[i] == pin)
            return i;
    return -1;
}
void BlinkBuzz::on(int pin)
{
    if (isAllowed(pin))
    {
        pinState[getPinIndex(pin)] = true;
        digitalWrite(pin, HIGH);
    }
}
void BlinkBuzz::off(int pin)
{
    if (isAllowed(pin))
    {
        pinState[getPinIndex(pin)] = false;
        digitalWrite(pin, LOW);
    }
}
void BlinkBuzz::onoff(int pin, int duration)
{
    if (isAllowed(pin))
    {
        on(pin);
        delay(duration);
        off(pin);
    }
}
void BlinkBuzz::onoff(int pin, int duration, int times)
{
    if (isAllowed(pin))
    {
        for (int i = 0; i < times; i++)
        {
            onoff(pin, duration);
            delay(duration);
        }
    }
}
void BlinkBuzz::onoff(int pin, int duration, int times, int pause)
{
    if (isAllowed(pin))
    {
        for (int i = 0; i < times; i++)
        {
            onoff(pin, duration);
            delay(pause);
        }
    }
}

#pragma endregion

#pragma region Asynchronous functions

void BlinkBuzz::update(int curMS)
{
    if (!enableAsync)
        return;

    if (curMS == -1)
        curMS = millis();

    for (int i = 0; i < numPins; i++)
    {
        if (curMS >= pinQ[i][pinQStart[i]])
        {
            int next = (pinQStart[i] + 1) % MAX_QUEUE;

            if (next == pinQIndef[i]) // if indefinite pattern, reset the queue
            {
                indefhelper(i);
                next = (pinQStart[i] + 1) % MAX_QUEUE;
            }

            if (pinQStart[i] != pinQEnd[i]) // if queue is not empty
            {
                // there is one exta "spacing" at the end of the queue. If the next thing is not EOQ, flip the state.
                // If it is, do nothing and quietly end the queue.
                if (next != pinQEnd[i])
                {
                    if (pinState[i])
                        off(allowedPins[i]);

                    else
                        on(allowedPins[i]);
                }
                pinQStart[i] = (pinQStart[i] + 1) % MAX_QUEUE;
            }
        }
    }
}

void BlinkBuzz::indefhelper(int idx)
{
    pinQStart[idx] = (pinQStart[idx] - 2) % MAX_QUEUE; // reset start of queue 2 spaces back

    int newTimeStamp = millis();
    int intermediateQIdx = (pinQStart[idx] + 1) % MAX_QUEUE;                                        // find the queue spot between the start and end
    int oldOnDuration = pinQ[idx][intermediateQIdx] - pinQ[idx][pinQStart[idx]];                    // find the duration
    int oldOffDuration = pinQ[idx][(pinQStart[idx] + 2) % MAX_QUEUE] - pinQ[idx][intermediateQIdx]; // find the other duration

    pinQ[idx][pinQStart[idx]] = newTimeStamp;                                                    // reset start timestamp to now
    pinQ[idx][intermediateQIdx] = newTimeStamp + oldOnDuration;                                  // hold ON for the onDuration time
    pinQ[idx][(pinQStart[idx] + 2) % MAX_QUEUE] = newTimeStamp + oldOnDuration + oldOffDuration; // hold OFF for the offDuration time
}

void BlinkBuzz::aonoffhelper(int idx, int timeStamp)
{
    pinQ[idx][pinQEnd[idx]] = timeStamp;
    pinQEnd[idx] = (pinQEnd[idx] + 1) % MAX_QUEUE;
}

// find the starting timestamp of an enqueing action based on the current queue state
int BlinkBuzz::calcTimeStamp(int pinIndex)
{
    double timeStamp;
    if (pinQStart[pinIndex] != pinQEnd[pinIndex]) // if queue is not empty, timeStamp = last queue time
    {
        timeStamp = pinQ[pinIndex][(pinQEnd[pinIndex] - 1) % MAX_QUEUE];
    }
    else
    {
        timeStamp = millis();
    }
    return timeStamp;
}

void BlinkBuzz::aonoff(int pin, int duration, bool overwrite)
{
    aonoff(pin, duration, 1, queueSpacing, overwrite);
}

void BlinkBuzz::aonoff(int pin, int duration, int times, bool overwrite)
{
    aonoff(pin, duration, times, duration, overwrite);
}

void BlinkBuzz::aonoff(int pin, int duration, int times, int pause, bool overwrite)
{
    int pinIndex = getPinIndex(pin);
    if (isAllowed(pin) && enableAsync)
    {
        if (overwrite && pinQStart[pinIndex] != pinQEnd[pinIndex])
            clearQueue(pin, LOW);

        int timeStamp = calcTimeStamp(pinIndex);
        if (times > 0)
        {
            if (pinQIndef[pinIndex] >= 0)
                clearQueue(pin, LOW);

            if (pinQStart[pinIndex] == pinQEnd[pinIndex])
                aonoffhelper(pinIndex, timeStamp);             // turn on
            for (int i = 0; i < times; i++)                    //
            {                                                  //
                aonoffhelper(pinIndex, timeStamp += duration); // turn off
                aonoffhelper(pinIndex, timeStamp += pause);    // buffer spacing
            }
        }
        else if (times == 0) // indefinite pattern
        {
            if (pinQStart[pinIndex] == pinQEnd[pinIndex])
                aonoffhelper(pinIndex, timeStamp);         // turn on
            aonoffhelper(pinIndex, timeStamp += duration); // turn off
            aonoffhelper(pinIndex, timeStamp += pause);    // buffer spacing
            pinQIndef[pinIndex] = pinQEnd[pinIndex];       // set indefinite pattern flag
        }
    }
}

void BlinkBuzz::clearQueue(int pin)
{
    if (isAllowed(pin) && enableAsync)
    {
        int idx = getPinIndex(pin);
        pinQStart[idx] = 0;
        pinQEnd[idx] = 0;
        pinQIndef[idx] = -1;
    }
}

void BlinkBuzz::clearQueue(int pin, int resetTo)
{
    if (isAllowed(pin) && enableAsync)
    {
        if (resetTo == LOW)
            off(pin);
        else if (resetTo == HIGH)
            on(pin);
        clearQueue(pin);
    }
}

#pragma endregion
