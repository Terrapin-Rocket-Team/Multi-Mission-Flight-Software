#ifndef BLINKBUZZ_H
#define BLINKBUZZ_H

#ifndef ARDUINO
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <Windows.h>

const uint64_t start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
enum val
{
	LOW,
	HIGH
};
inline uint64_t millis() { return (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - start); }
inline void digitalWrite(int pin, val value)
{
	int color;
	switch (pin)
	{
	case 13:
		color = 36;
		break;
	case 33:
		color = 33;
		break;
	case 32:
		color = 95;
		break;
	default:
		color = 0;
		break;
	}
	printf("\x1B[%dm%.3f - %d to \x1B[%dm%s\x1B[0m\n", color, millis() / 1000.0, pin, value == LOW ? 91 : 92, value == LOW ? "LOW" : "HIGH");
}
inline void delay(uint64_t ms)
{
	Sleep(ms);
}

#else

#include <Arduino.h>

#endif // !ARDUINO

class BlinkBuzz
{
public:
	BlinkBuzz(int *allowedPins, int numPins, bool enableAsync);
	~BlinkBuzz();

	void update(int curMS = -1);

	void on(int pin);  // turn on a pin
	void off(int pin); // turn off a pin

	void onoff(int pin, int duration);						 // turn on a pin for a ms duration
	void onoff(int pin, int duration, int times);			 // turn on a pin for a ms duration, repeat times times with ms duration pause
	void onoff(int pin, int duration, int times, int pause); // turn on a pin for a ms duration, repeat times times with ms pause pause

	// turn on a pin for a ms duration asynchronously, overwriting current queue or adding to the end
	void aonoff(int pin, int duration, bool overwrite = false);
	// turn on a pin for a ms duration, repeat `times` times with ms `duration` pause asynchronously, overwriting current queue or adding to the end
	void aonoff(int pin, int duration, int times, bool overwrite = false);
	// turn on a pin for a ms duration, repeat `times` times with ms `pause` pause asynchronously, overwriting current queue or adding to the end
	void aonoff(int pin, int duration, int times, int pause, bool overwrite = false);

	void clearQueue(int pin);
	void clearQueue(int pin, int resetTo);

private:
	static const int MAX_QUEUE = 50;
	int numPins;

	// each pin has its own queue of actions
	int *allowedPins; // list of pins that are allowed to be used (prevents misfiring pins on accident thru typos)
	bool *pinState;	  // current state of each pin
	int *pinQStart;	  // each pin's queue start index (i.e. the next timestamp to be executed)
	int *pinQEnd;	  // each pin's queue end index (i.e. the last timestamp to be executed)
	int *pinQIndef;	  // each pin's queue indefinite repeat index (i.e. the timestamp at which to restart an indefinite pattern)

	int **pinQ; // each pin's queue of timestamps

	//[p1] [time][time][time]...[time]
	//[p2] [time][time][time]...[time]
	//[p3] [time][time][time]...[time]
	// ...
	//[pn] [time][time][time]...[time]

	bool isAllowed(int pin);
	int getPinIndex(int pin);
	void indefhelper(int idx);
	void aonoffhelper(int idx, int timeStamp);
	int calcTimeStamp(int pinIndex);
	bool enableAsync;

	int queueSpacing = 100; // ms between each new action when you don't specify a # of repeats AND append a new action before the current one completes.
};

extern BlinkBuzz bb;
extern int BUZZER;
extern int LED;
#endif // !BLINKBUZZH
