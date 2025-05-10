# Basic Usage

!!! warning "Important!"
    In order for PIO to recognize that you are working on a PIO project, you *must* open VSCode in the root directory of that project. that is, the directory that has the `platformio.ini` file in it. Without this, PlatformIO **will not initialize** and you will be unable to build or use proper Intellisense.

---

## Intro

There are very few things that need to be done in order for the absolute minimum requirements to use MMFS to be met. You *must*: Extend the `State` class, pass it to an `MMFSSystem` object, and call the `init` and `update` methods during setup and loop, respectively. There are, of course, many more things that you *can* do to tailor MMFS to your preferences, but we'll start slow.

---

## Initial Integration


### Overriding `State`
Let's start by extending `State`. In your `src` folder, make a new set of files called something like `NewState.h` and `NewState.cpp`.

Paste these contents into them:

/// tab | NewState.h
```cpp title=""
#ifndef NEWSTATE_H
#define NEWSTATE_H

#include <State/State.h>

using namespace mmfs;
class NewState : public State {
    public:
        NewState(Sensor **sensors, int numSensors, Filter *filter);
        void determineStage() override;
};

#endif
```
///

/// tab | NewState.cpp
```cpp title=""
#include "NewState.h"

NewState::NewState(Sensor **sensors, int numSensors, Filter *filter) : State(sensors, numSensors, filter) {}

void NewState::determineStage() // (1)!
{
    // Add whatever stage determination logic you'd like here.
    // Here is one simplified example:
    if (stage == 0 && acceleration.z() > 10 && position.z() > 20) 
    {                                                             // accelerating upwards and off the pad
        stage = 1;                                                // ascent
        getLogger().setRecordMode(FLIGHT);
    }                                                             
    else if (stage == 1 && velocity.z() < 0)                      // descending
        stage = 2;                                                
    else if (stage == 2 && position.z() < 20)                     
    {                                                             // landed
        stage = 3;
        getLogger().setRecordMode(GROUND);
    }
}
``` 
{.annotate}

1. You may have as many stages as you like, but MMFS expects you to have at least launch and land stages.
/// 

Let's briefly talk about what we've done. `State` declares a method called `#!cpp void determineStage()` that is called every time it updates. Only MMFS doesn't know which stages you might care about, so you have to tell it by overriding that function. The reason it's so important to detect launch and landing is because of those calls to `getLogger().setRecordMode()`. This changes the rate at which MMFS records data, but don't worry, we'll get to all of that. Just understand that this is an important method that must make those calls to `getLogger()` in order for data logging to work the way you want it to.[^1]

### Instantiating the `State` object

Now that we have our `State` object with its `determineStage` method, we can head back over to the main file to finish our integration. We need to create an actual instance of the object for us to use in the `setup` and `loop` methods. You'll notice that in the constructor we implemented, it took a `Sensor **`. If you're not all that familiar with pointers, this is basically saying "an array of pointers to `Sensor` objects". This is because we need to pass in all of the sensors that we want to use. For now, let's just use the included 'IMU', 'GPS', and 'Barometer' sensors. We can do this by creating an array of pointers to `Sensor` objects, and then passing that to the `State` constructor. There was another important parameter, the `Filter`, which is a class that we haven't talked about yet. For now, we can just pass in `nullptr` for that parameter, as we don't need to use it right now.

```cpp title="main.cpp"
#include <Arduino.h>
#include "MMFS.h"
#include "NewState.h"

MAX_M10S gps;
DPS310 baro;
BMI088andLIS3MDL imu9dof;

Sensor *sensors[] = {&gps, &baro, &imu9dof};

NewState state(sensors, 3, nullptr);

void setup()
{

}

void loop()
{
  
}
```

Here we've used the sensors that come on the Avionics' sub-team's Sensor Board v1.1, which anyone else on the team is also welcome to use. If you need to implement your own sensors, you can check out the [Sensor documentation](#) for more information.

### Creating an `MMFSConfig` object

Now, we need to create an `MMFSConfig` object, which is a neat little object that follows what's called a "builder pattern". This means that we can set all of the configuration options that we want, while leaving out the options we don't care about. For our use case, the only thing we *need* to send it is the state object, but we'll set up a couple other things as well so you get an idea of the object's versatility. Here's what that looks like:

```cpp title="main.cpp" linenums="8" hl_lines="6-9"
...
Sensor *sensors[] = {&gps, &baro, &imu9dof};

NewState state(sensors, 3, nullptr);

MMFSConfig config = MMFSConfig()
                        .withState(&state)
                        .withBuzzerPin(13)
                        .withBBPin(LED_BUILTIN);

void setup()
{
...
```

So here, we're just setting the buzzer pin to 13 and the built-in LED pin to `LED_BUILTIN`, which is a constant that is defined in the Arduino framework. You can set any of the configuration options that you want by appending their methods to the end of the list, but for now, we'll just leave it at that. See the [MMFSConfig documentation](#) for more information.

### Creating an `MMFSSystem` object

Now that we have our config, we need ot actually create the `MMFSSystem` object. This is a simple one-liner:

```cpp title="main.cpp" linenums="13" hl_lines="7"
...
MMFSConfig config = MMFSConfig()
                        .withState(&state)
                        .withBuzzerPin(13)
                        .withBBPin(LED_BUILTIN);

MMFSSystem system(config);

void setup()
{
...
```

### Calling `init` and `update`

Now, we just need to call the `init` and `update` methods in the `setup` and `loop` methods, respectively. MMFS will take care of everything else. Yes, really. This is all the code you need to write to get a minimum working system up and running. 

/// tab | New code
```cpp title="main.cpp" linenums="17" hl_lines="5 10"
MMFSSystem computer = MMFSSystem(&config);

void setup()
{
  computer.init();
}

void loop()
{
  computer.update();
}
```
///

/// tab | Full main.cpp
```cpp title=""
#include <Arduino.h>
#include "MMFS.h"
#include "NewState.h"

MAX_M10S gps;
DPS310 baro;
BMI088andLIS3MDL imu9dof;

Sensor *sensors[] = {&gps, &baro, &imu9dof};

NewState state(sensors, 3, nullptr);

MMFSConfig config = MMFSConfig()
                        .withState(&state)
                        .withBuzzerPin(13)
                        .withBBPin(LED_BUILTIN);

MMFSSystem computer = MMFSSystem(&config);

void setup()
{
  computer.init();
}

void loop()
{
  computer.update();
}
```
///

### Conclusion

That's it! You now have a fully functional MMFS system that is ready to be used. You can add more sensors, change the configuration options, and customize the `NewState` class to your heart's content. See the [MMFSConfig documentation](#) for more information on what you can do with the config object, and see the [State documentation](#) for more information on how to customize the state object.

---

## Easy Modifications

Here are few easy modifications that you can make to the system to make it more suited to your needs. If you don't see what you're looking for here, try the page relating to that specific feature, or reach out to someone on Avionics.

### Modifying the update rate

You can modify the update rate of the system by using one of two methods on the MMFS Config object. They both have the same effect, and the last one called is the one used.

/// tab | withUpdateRate()
```cpp title="" linenums="14" hl_lines="5"
MMFSConfig config = MMFSConfig()
                        .withState(&state)
                        .withBuzzerPin(13)
                        .withBBPin(LED_BUILTIN)
                        .withUpdateRate(10); // 10Hz (100 ms between updates)
```
///
/// tab | withUpdateInterval()
```cpp title="" linenums="14" hl_lines="5"
MMFSConfig config = MMFSConfig()
                        .withState(&state)
                        .withBuzzerPin(13)
                        .withBBPin(LED_BUILTIN)
                        .withUpdateInterval(100); // 100ms between updates (10Hz)
```
///

!!! note
    10 hz is the default rate. If you set it too high, the system may struggle to keep up, so we recommend keeping it at 50 hz or lower.

### Sensor drift correction

While on the pad waiting for launch, it is very likely that the sensors will begin to drift slightly. There is a feature in MMFS that allows you to correct for this until launch is detected. This correction acts by averaging out some length of sensor data, while ignoring the very most recent data. As most launch detection systems require the rocket to be noticeably off the ground and may take a few iterations to properly detect launch, the idea behind this was to stop early launch data from affecting the zeroing of the sensors.

!!! danger
    Using this feature **requires** that you have working launch detection. If not, they will continuously attempt to correct themselves to zero, and thus report faulty data for the duration of the flight.

```cpp title="" linenums="14" hl_lines="5-7"
MMFSConfig config = MMFSConfig()
                        .withState(&state)
                        .withBuzzerPin(13)
                        .withBBPin(LED_BUILTIN)
                        .withUsingSensorBiasCorrection(true); // (1)!
                        .withSensorBiasCorrectionDataLength(2); // two seconds of data to average over, BUT
                        .withSensorBiasCorrectionIgnore(1); // ignore the most recent 1 second of data.
```
{.annotate}

1. This is false by default, however the length of collection and ignoring default to the two values below, meaning you do not have to call the methods if these values are fine for you.

### Using BlinkBuzz

As we saw earlier, you must add the signaling pins you want to use to the `MMFSConfig` object. This is done by calling the `withBuzzerPin()` and `withBBPin()` methods. The buzzer pin is special and has some default events that utilize it. `withBBPin()` can be use to add any pin (or a second buzzer). You can add up to 50 pins.

To use a pin, you can call `#!cpp bb.onoff(int pin, int duration, int repeat = 1, int pause = duration)`. If you are in the `loop` phase of the program, you can use `#!cpp bb.aonoff(...)` to do the same thing asynchronously (i.e. beep for long periods without blocking other code execution).

See the [BlinkBuzz documentation](#) for more usage information.

### Using the Logger

You can modify the format of the event logger by using the `withLogPrefixFormatting()` method. This takes a string that must include the `$time` and `$logType` (case specific) keywords. It will then format all log entries accordingly. The default is 

`"$time - [$logType] "`

To use the logger to record data, you can call `#!cpp getLogger().recordLogData(LogType type, const char *format, ...)`. This will log the data to both the SD card and USB serial port.

See the [Logger documentation](#) for more information.

## Conclusion

This just scratches the surface of what these utilities and systems can handle (blink buzz can do infinite asynchronous morse code! ...if you need that).

See the [MMFSConfig documentation](#) for more information on what you can do with the config object.

See the [State documentation](#) for more information on how to customize the state object.

Also check out the [Event system](#), [Sensor interface](#), [DataReporter interface](#), and [Filter interface](#) for more information on how to use some more advanced parts of the system.

[^1]: There are other configuration options that you can use to disregard the stage system, but we don't recommend it. Check out the [Logger documentation](#) for more information.

