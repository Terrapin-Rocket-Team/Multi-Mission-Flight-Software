# Sensor

The Sensor interface is the core purpose of MMFS. It allows us to reuse code across subteams without reinventing the wheel. In the past, Avionics developed a sensor board with a GPS, Barometer, and 9DoF IMU that all three SRAD electronics teams on the competition rocket used.

The Sensor interface extends DataReporter, and further  breaks down into multiple different types of sensors, each with their own functionality.

---

## Main Points

1. `update()` and `begin()`
    
    These two methods allow all sensors to be initialized or updated from one large loop. ~~They internally call `init()` and `read()`.~~ You can specify whether the sensor should use BiasCorrection or not in the `begin(bool useBiasCorrection)` method.
2. `init()` and `read()`
    These are designed to be hte methods that users implement to read the physical hardware sensors. In our implementation of each sensor, these two functions retrieve data from the hardware and store them  in internal variables, and the `update()` and `begin()` functions do any kind of transformations on that data that need to be done before they can be passed to the State for processing.

---

## Implementation

There are currently 6 types of sensors:

```
BAROMETER_,
GPS_,
IMU_,
LIGHT_SENSOR_,
ENCODER_,
OTHER_
```

MMFS provides basic implementations of each of these, with the exception of `OTHER_`. If you have your own sensor to implement, determine first if it can fit under the umbrella of one of these types. If so, you may save yourself a lot of work. If not, you can utilize the `OTHER_` category and create your own implementation.

/// tab | Implementing an Existing Type

First, find the source file and the related docs file for the type you with to implement.

- [Barometer](baro.md)
- [Encoder](enc.md)
- [GPS](gps.md)
- [IMU](imu.md)
- [LightSensor](light.md)

Next, find a library online that interfaces with your sensor of choice. We use Adafruit's and SparkFun's libraries mostly, but there are many of them out there. This makes it much easier to interface with the hardware, allowing you to focus on functionality without worrying about writing to individual registers. When you find one, great!

Go into the docs for the type of sensor your using and see how we set them up. Essentially, you should only have to override the init() and read() methods, allowing you to quickly get up and running with your new sensor.

!!! note

    As an aside, almost every sensor we use has I2C capabilities that we leverage. You may see specific code related to that, but if yours uses SPI or some other communication interface, don't worry. You can still use these pre-built interfaces.

///


/// tab | Creating Your Own Type

///