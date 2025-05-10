---
title: User Manual - Introduction
---
# The Complete MMFS User's Manual

!!! info
    These docs were written in the order shown on the sidebar. You many notice that later docs somewhat rely on the reader understanding topics covered earlier. They should mostly be self-sufficient, but it's not guaranteed.

    As always, if you have any questions, feel free to reach out on Slack or Github.
---

<sub><sup><sub><sup>Now on a cool new website!</sup></sub></sup></sub>

This is the official user's manual for TRT's Multi-Mission Flight Software (MMFS). It is designed to be a comprehensive guide for users of all levels, from beginners to advanced developers. The manual covers everything from installation and setup to advanced features and troubleshooting.

The library is essentially split into two categories: **Utilities** and **Interfaces**. 

---

## Utilities

We describe utilities as any part of the library designed to be used as-is, without the end user (you) having to overload or otherwise modify it.
The utilities offered by MMFS are:

### **[BlinkBuzz](blinkbuzz.md)**
An (asynchronous!) utility for outputting patterned buzzes and LED blinks.

### **[CircBuffer](circbuf.md)**
A queue or FIFO system for storing data in a fixed-size buffer.

### **[Logger](logger.md)**
A simple logging utility that can log to the SD card or USB serial port.

### **[RetrieveData](retrieve-data.md)**
A system that allows transfer of flight data files over Serial, allowing retrieval of files stored in onboard flash.

### **[Math](math.md)**
A collection of ... math? ... objects (vectors, matrices, and quaternions) and their functions.

### **[MMFSSystem](mmfssys.md)**
An object designed to handle all of the MMFS functions during flight.

Most of the power that MMFS offers comes from the Logger and MMFSSystem utilities. That is to say, those are two of the most complicated systems in the library.

---

## Interfaces

Interfaces are for any other part of the library that is not fully implemented, or otherwise is expected to be modified by the end user. Most of them are not strictly interfaces, but rather abstract classes. The following interfaces are offered by MMFS:

### **[State](state.md)**
This is the main purpose of using MMFS. It is an abstract class that is used to define the state of the rocket. It is designed to be overloaded by the end user, and is used to define the behavior of the telemetry systems during flight.

### **[DataReporter](data-reporter.md)**
This is an interface for recording flight CSV data using Logger. State implements it, as do all of the sensors. You may extend it to add your own data reporters.

### **[Sensor](sensor.md)**
This is the base sensor interface. There are further interfaces for each type of sensor:

- **[Barometer](baro.md)**
- **[IMU](imu.md)** (9DOF, including accelerometer, gyroscope, and magnetometer)
- **[GPS](gps.md)**
- **[Encoder](enc.md)** (a motor encoder)
- **[LightSensor](light.md)**

### **[Event](event.md)**
This is a simple, readily-extendable event system. You can create classes that listen to and fire off custom events, or overload the handler for the default events.

### **[Filters](filters.md)**
This is a basic Kalman filter interface, able to be passed into State for it to use to filter sensor data.