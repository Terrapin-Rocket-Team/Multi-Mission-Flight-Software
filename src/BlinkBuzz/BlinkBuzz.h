#ifndef BLINKBUZZ_H
#define BLINKBUZZ_H


#ifndef ARDUINO
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <Windows.h>

const uint64_t start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
enum val { LOW, HIGH };
inline uint64_t millis() { return (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - start); }
inline void digitalWrite(int pin, val value) {
	printf("%.3f - %d to %s\n", millis()/ 1000.0, pin, value == LOW ? "LOW" : "HIGH");
}
inline void delay(uint64_t ms) {
	Sleep(ms);
}

#else

#include <Arduino.h>

#endif // !ARDUINO


class BlinkBuzz
{
public:
	BlinkBuzz(int* allowedPins, int numPins, bool enableAsync);
	~BlinkBuzz();

	void update(int curMS = -1);

	void on(int pin);//turn on a pin
	void off(int pin);//turn off a pin

	void onoff(int pin, int duration);//turn on a pin for a ms duration
	void onoff(int pin, int duration, int times);//turn on a pin for a ms duration, repeat times times with ms duration pause
	void onoff(int pin, int duration, int times, int pause);//turn on a pin for a ms duration, repeat times times with ms pause pause
	
	void aonoff(int pin, int duration);//turn on a pin for a ms duration asynchronously
	void aonoff(int pin, int duration, int times);//turn on a pin for a ms duration, repeat times times with ms duration pause asynchronously
	void aonoff(int pin, int duration, int times, int pause);//turn on a pin for a ms duration, repeat times times with ms pause pause asynchronously
	
	void clearQueue(int pin);
	void clearQueue(int pin, int resetTo);

private:
	static const int MAX_QUEUE = 100;
	int numPins;
	int* allowedPins;
	bool* pinState;
	int* pinQStart;
	int* pinQEnd;
	int** pinQ;

	//[p1] [time][time][time]...[time]
	//[p2] [time][time][time]...[time]
	//[p3] [time][time][time]...[time]
	// ...
	//[pn] [time][time][time]...[time]

	bool isAllowed(int pin);
	int getPinIndex(int pin);
	void aonoffhelper(int pin, int timeStamp);
	bool enableAsync;
};

extern BlinkBuzz bb;
extern int BUZZER;
extern int LED;
#endif // !BLINKBUZZH
