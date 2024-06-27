
#include "BlinkBuzz.h"



BlinkBuzz::BlinkBuzz(int* allowedPins, const int numPins, bool enableAsync)
{
	this->enableAsync = enableAsync;
	this->numPins = numPins;
	this->allowedPins = allowedPins;
	pinState = new bool[numPins];
	if (enableAsync) {
		pinQStart = new int[numPins];
		pinQEnd = new int[numPins];
		pinQ = new int* [numPins];
		for (int i = 0; i < numPins; i++)
		{
			pinState[i] = false;
			pinQStart[i] = 0;
			pinQEnd[i] = 0;
			pinQ[i] = new int[MAX_QUEUE];
		}
	}
	else {
		pinQ = nullptr;
		pinQStart = nullptr;
		pinQEnd = nullptr;
	}
	for (int i = 0; i < numPins; i++)
	{
		pinState[i] = 0;
#ifdef ARDUINO
		pinMode(allowedPins[i], OUTPUT);
#endif // ARDUINO
	}
}
BlinkBuzz::~BlinkBuzz()
{
	delete[] pinState;
	if (enableAsync) {
		delete[] pinQStart;
		delete[] pinQEnd;
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
	if (pin < 0) return false;

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
	if (!enableAsync) return;

	if (curMS == -1)
		curMS = millis();

	for (int i = 0; i < numPins; i++)
	{
		if (pinQStart[i] != pinQEnd[i])
		{
			if (curMS >= pinQ[i][pinQStart[i]])
			{
				if (pinState[i])
					off(allowedPins[i]);

				else
					on(allowedPins[i]);

				pinQStart[i]++;
				if (pinQStart[i] == MAX_QUEUE)
					pinQStart[i] = 0;
			}
		}
	}
}

void BlinkBuzz::aonoffhelper(int idx, int timeStamp)
{
	pinQ[idx][pinQEnd[idx]] = timeStamp;
	pinQEnd[idx]++;
	if (pinQEnd[idx] == MAX_QUEUE)
		pinQEnd[idx] = 0;
}
void BlinkBuzz::aonoff(int pin, int duration)
{
	int pinIndex = getPinIndex(pin);
	if (isAllowed(pin) && enableAsync && pinIndex >= 0)
	{
		int t = millis();
		aonoffhelper(pinIndex, t);
		aonoffhelper(pinIndex, t + duration);
	}
}
void BlinkBuzz::aonoff(int pin, int duration, int times)//Need to change duration to send increasign values so that it is a timestamp and not a duration. figure out how to do this tomorrow.
{
	int pinIndex = getPinIndex(pin);
	if (isAllowed(pin) && enableAsync && pinIndex >= 0)
	{
		double timeStamp = millis();
		aonoffhelper(pinIndex, timeStamp);
		for (int i = 0; i < times; i++)
		{
			aonoffhelper(pinIndex, timeStamp += duration);
			if(i != times - 1) aonoffhelper(pinIndex, timeStamp += duration);

		}
	}
}
void BlinkBuzz::aonoff(int pin, int duration, int times, int pause)
{
	int pinIndex = getPinIndex(pin);
	if (isAllowed(pin) && enableAsync && pinIndex >= 0)
	{
		int timeStamp = millis();
		aonoffhelper(pinIndex, timeStamp);
		for (int i = 0; i < times; i++)
		{
			aonoffhelper(pinIndex, timeStamp += duration);
			if (i != times - 1) aonoffhelper(pinIndex, timeStamp += pause);
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
		else if(resetTo == HIGH)
			on(pin);
		clearQueue(pin);
	}
}

#pragma endregion

