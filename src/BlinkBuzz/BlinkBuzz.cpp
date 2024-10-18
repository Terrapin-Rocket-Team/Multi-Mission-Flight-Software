
#include "BlinkBuzz.h"
#include "Error/ErrorHandler.h"

#ifndef ARDUINO
#include "NativeTestHelper.h"
#else

#include <Arduino.h>

#endif // !ARDUINO

BlinkBuzz::BlinkBuzz(int *allowedPins, const int numPins, bool enableAsync, int maxQueueSize)
{
    this->enableAsync = enableAsync;
    this->numPins = numPins;
    this->allowedPins = allowedPins;
    this->maxQueueSize = maxQueueSize;
    pinState = new bool[numPins];
    if (enableAsync)
    {
        pinIsIndef = new bool[numPins];
        pinQStart = new int[numPins];
        pinQEnd = new int[numPins];
        pinQ = new int *[numPins];
        for (int i = 0; i < numPins; i++)
        {
            pinQStart[i] = 0;
            pinQEnd[i] = 0;
            pinIsIndef[i] = false;
            pinQ[i] = new int[maxQueueSize];
        }
    }
    else
    {
        pinQ = nullptr;
        pinQStart = nullptr;
        pinQEnd = nullptr;
        pinIsIndef = nullptr;
    }
    for (int i = 0; i < numPins; i++)
    {
        pinState[i] = false;
#ifdef ARDUINO
        pinMode(allowedPins[i], OUTPUT); // INPUT is otherwise defined on windows, so can't make a stub for it
#endif
    }
}
BlinkBuzz::~BlinkBuzz()
{
    delete[] pinState;
    if (enableAsync)
    {
        delete[] pinQStart;
        delete[] pinQEnd;
        delete[] pinIsIndef;
        for (int i = 0; i < numPins; i++)
        {
            delete[] pinQ[i];
        }
        delete[] pinQ;
    }
}

#pragma region Synchronous and Helper functions

bool BlinkBuzz::isOn(int pin)
{
    if (isAllowed(pin))
        return pinState[getPinIndex(pin)];
    errorHandler.addError(mmfs::ErrorType::NONCRITICAL_WARNING, "BlinkBuzz: Attempted to check the state of an unallowed pin", pin);
    return false;
}

bool BlinkBuzz::isUsingAsync()
{
    return enableAsync;
}

bool BlinkBuzz::isAllowed(int pin)
{
    if (pin > 0)
        for (int i = 0; i < numPins; i++)
            if (allowedPins[i] == pin)
                return true;
    errorHandler.addError(mmfs::ErrorType::NONCRITICAL_WARNING, "BlinkBuzz: Attempted to use an unallowed pin", pin);
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
void BlinkBuzz::onoff(int pin, BBPattern &pattern)
{
    if (isAllowed(pin))
    {
        BBPattern::BBPatternNode *cur = pattern.head;
        while (cur != nullptr)
        {
            toggle(pin);
            delay(cur->duration);
            cur = cur->next;
        }
    }
}
void BlinkBuzz::toggle(int pin)
{
    if (isAllowed(pin))
    {
        if (pinState[getPinIndex(pin)])
            off(pin);
        else
            on(pin);
    }
}

#pragma endregion

#pragma region Asynchronous functions

void BlinkBuzz::update(int curMS)
{
    if (!enableAsync){
        errorHandler.addError(mmfs::ErrorType::NONCRITICAL_WARNING, "BlinkBuzz: Attempted to use an asynchronous function without enabling asynchronous mode.");
        return;
    }

    if (curMS == -1)
        curMS = millis();

    for (int i = 0; i < numPins; i++) // for each pin
    {
        if (curMS >= pinQ[i][pinQStart[i]]) // if the current time is greater than the next action time
        {
            int nextQStart = (pinQStart[i] + 1) % maxQueueSize;
            if (pinQStart[i] != pinQEnd[i] && nextQStart != pinQEnd[i]) // if the queue is not empty
            {
                toggle(allowedPins[i]); // toggle the pin
                pinQStart[i] = nextQStart;
            }

            if (nextQStart == pinQEnd[i] && pinIsIndef[i]) // if the queue is empty and the pattern is indefinite
            {
                pinQStart[i] = 0;
                indefHelper(i, curMS);
                update(curMS);
            }
        }
    }
}

void BlinkBuzz::indefHelper(int pin, double curMS)
{
    double originalTimeStamp = pinQ[pin][0];
    pinQ[pin][0] = curMS; // update the timestamp to be relative to the current time
    for (int i = 1; i < pinQEnd[pin]; i++)
    {
        pinQ[pin][i] = curMS + pinQ[pin][i] - originalTimeStamp;
    }
}

// add a timestamp to a pin's queue
void BlinkBuzz::aonoffhelper(int idx, int timeStamp)
{
    if(pinQEnd[idx] == maxQueueSize - 1){ // if the queue is full, don't add anything
        errorHandler.addError(mmfs::ErrorType::NONCRITICAL_WARNING, "BlinkBuzz: A pin's queue has overflown.", allowedPins[idx]);
        return;
    }
    pinQ[idx][pinQEnd[idx]] = timeStamp;
    pinQEnd[idx] = (pinQEnd[idx] + 1) % maxQueueSize;

}

// find the starting timestamp of an enquing action based on the current queue state
int BlinkBuzz::calcTimeStamp(int pin, int pinIndex)
{
    double timeStamp = millis();
    if (pinQStart[pinIndex] != pinQEnd[pinIndex])
    { // if queue is not empty, add spacing
        timeStamp += pinQ[pinIndex][(pinQEnd[pinIndex] - 1) % maxQueueSize] + queueSpacing;
    }
    return timeStamp;
}

void BlinkBuzz::aonoff(int pin, int duration)
{
    aonoff(pin, duration, 1);
}

void BlinkBuzz::aonoff(int pin, int duration, int times)
{
    aonoff(pin, duration, times, duration);
}

void BlinkBuzz::aonoff(int pin, int duration, int times, int pause)
{
    BBPattern pattern(duration, times == 0 ? 1 : times, pause);
    aonoff(pin, pattern, times == 0);
}

void BlinkBuzz::aonoff(int pin, const BBPattern &pattern, bool indef)
{
    int pinIndex = getPinIndex(pin);
    if (isAllowed(pin) && enableAsync)
    {
        clearQueue(pin);

        int timeStamp = calcTimeStamp(pin, pinIndex);
        BBPattern::BBPatternNode *cur = pattern.head;
        if (cur != nullptr) // if pattern is not empty
        {
            if (!pinState[pinIndex])               // if the pin is off, turn it on
                aonoffhelper(pinIndex, timeStamp); // add the first action (TURN_ON)
            while (cur != nullptr)                 // add all actions
            {
                // if (cur->next == nullptr) break; // if the next action is null, break to not add the last action
                aonoffhelper(pinIndex, timeStamp += cur->duration); // add the next action
                cur = cur->next;
            }
            if (indef)
                pinIsIndef[pinIndex] = true;
            else
                pinIsIndef[pinIndex] = false;
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
