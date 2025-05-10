# BlinkBuzz

This utility is designed to be used as-is, without the end user (you) having to overload or otherwise modify it. It is an asynchronous utility for outputting patterned buzzes and LED blinks. It can be used to create simple patterns, such as beeping a certain number of times, or more complex patterns, such as SOS in morse code. It can also be used to create patterns that repeat indefinitely or patterns that repeat a specific number of times. It is designed to be easy to use and flexible enough to handle a wide variety of use cases.

## Setup

We strongly recommend that you use the `MMFSSystem` and `MMFSConfig` objects whenever you use MMFS. That said, all utilities have the ability to be used outside of `MMFSSystem`, including BlinkBuzz. The only difference between the two is the setup. All other functions are identical.

/// tab | With MMFSSystem

BlinkBuzz does not have too many configuration options. Here they are, shown with their default values if they have them:

```c++
#include <MMFS.h>

int GPS_STATUS_PIN = 25;

MMFSConfig config = MMFSConfig()
                    .withBuzzerPin(int pinNum) // no default
                    .withBBPin(int pinNum) // add any pin you'd like, no default
                    .withBBPin(int pinNum) // add as many pins as you want, no default
                    .withBBAsync(true, 50); // allow async patterns, max queue size of 50.

MMFSSystem system = MMFSSystem(config);
```

In `setup()` you *must* call `system.init()` to initialize the system. This will also initialize the BlinkBuzz utility.

In order to take advantage of the async features, you must call `system.update()` in your `loop()`. This will also update the BlinkBuzz utility.
///

/// tab | Without MMFSSystem

If you do not wish to use the MMFSSystem, you can use BlinkBuzz as a standalone utility. You must however set up the object yourself, and remember to call `bb.update()` in your `loop()`. 

```c++
#include <BlinkBuzz.h>

int *allowedPins[] = { LED_BUILTIN, 33 };
BlinkBuzz bb; //define a global variable that is declared in the Blinkbuzz.h file for easy access across files

double lastTime = 0; // See loop()

setup() {
    bb.init(allowedPins, 2, true, 50); // allowed pins, number of pins, allow usage of async patterns, Max queue size per pin (async mode only)
    //NOTE: Max queue size refers to the number of on and off toggles that are performed, so ON -> OFF is 2 spots in the queue.
}

loop()
{
    bb.update(); // The more frequently this is called, the more accurate the timing will be.

    // We recommend not using a delay() call for loop iterations when using async patterns.
    // Instead, maybe try something like this:
    
    double time = millis();
    if(time - lastTime < DESIRED_INTERVAL) // however much you would have otherwise delay()ed for
        return;
    lastTime = time; // lastTime = global variable
    
    // the rest of your loop code here
}
```
///

!!! warning
    There is a moderate memory overhead associated with using async patterns. Keep queue sizes as small as is reasonable.

---

## Usage

In any file that includes BlinkBuzz, you can use any of the BlinkBuzz functions. 

### Synchronous usage

These are the calls you would make while in `setup()` or if you disable async usage.

!!! note
    `BBPattern` arguments are not supported in synchronous function calls, nor are they planned to be in the future.

```c++ title=""
#include <BlinkBuzz.h>

void setup() {

    // Simply hold the pin on/off:
    bb.on(BUZZER);  // turn on
    bb.off(BUZZER); // turn off

    //Patternize
    bb.onoff(BUZZER, 200, 3, 100); // beep 3 times, 200ms on, 100ms off
    // OR
    bb.onoff(BUZZER, 200, 5);      // beep 5x, 200ms on, 200ms off
    // OR
    bb.onoff(BUZZER, 200);         // beep 1x, 200ms on
}
```

### Async Usage

Asynchronous use was the reason BlinkBuzz came to be in the first place, and is what makes the utility so valuable and powerful. You can blink lights in custom patterns indefinitely, without blocking the rest of your code. This is invaluable during testing as the buzzer can beep to indicate state changes without stopping the State from updating.

#### Old-School

Here are the most basic examples of how to use the async functions:
```c++ title=""

    bb.aonoff(BUZZER, 200, 3, 100); // beep 3 times, 200ms on, 100ms off
    // OR
    bb.aonoff(BUZZER, 200, 5);      // beep 5x, 200ms on, 200ms off
    // OR
    bb.aonoff(BUZZER, 200);         // beep 1x, 200ms on

    // Can clear a pin's queue:
    bb.clearQueue(BUZZER);
```

#### BBPattern {#bbpattern}

You can also use the `BBPattern` class to build complex patterns. You define a pattern  with the same arguments as `onoff()`, but you can also append patterns to each other, and add "rests" to the end of a pattern. You may then call the pattern with `aonoff()` with any pin.

```cpp title=""
    BBPattern pattern = BBPattern(ON_DURATION, REPEATS, OFF_DURATION);
    // For example, to build an SOS pattern, build the S and O letters:
    BBPattern s(50, 3, 200);
	BBPattern o(500, 3, 200);
    // Then, append them to each other to build a single SOS pattern using the `a()` function:
    BBPattern sos;
    sos.a(s).a(o).a(s);
    //NOTE: do not do BBPattern sos = s.a(o).a(s); as this will not work as expected. Minor bug.
    // Then call it
    bb.aonoff(BUZZER, sos);

    // To repeat a pattern indefinitely, use the following:
    bb.aonoff(BUZZER, sos, true);

    // You can also append a "rest" to the end of a pattern, changing it's final duration.
    bb.aonoff(BUZZER, sos.r(1000), true); // Rest for 1 second between the SOS patterns
    //The rest can be added inline like this or to the pattern object itself.

    // Appending patterns and adding rests can be done in any order as many times as you wish (so long as you have the queue space to hold them)
}
```